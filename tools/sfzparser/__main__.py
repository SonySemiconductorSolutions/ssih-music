# SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
#
# Copyright 2022 Sony Semiconductor Solutions Corporation
#

import argparse
import os
import re
import subprocess

from . import core

parser = argparse.ArgumentParser(
    prog='sfzparser',
    description='Convert SFZ files for use in ssprocLib'
)
parser.add_argument('filename', nargs='+')
parser.add_argument('-o', '--out', default='output')

args = parser.parse_args()


def convertNoteToInt(note):
    """Convert note name to integer

    Args:
        note: A note name. For example C4.

    Returns:
        The note number. If note name is C4, returns 60.
    """
    if note.isnumeric():
        return int(note)
    elif note[0:1] == '-':
        return int(note)
    elif note:
        base = [69, 71, 60, 62, 64, 65, 67]
        pitch = base[ord(note[0:1].lower()) - ord('a')]
        sym = note[1:2]
        if sym == '#':
            return (pitch + 1) + (int(note[2:]) - 4) * 12
        elif sym == 'b':
            return (pitch - 1) + (int(note[2:]) - 4) * 12
        else:
            return (pitch + 0) + (int(note[1:]) - 4) * 12


def getAudioFs(input_path):
    # FFMPEG_ARGS = '-hide_banner -loglevel error -y'
    FFMPEG_ARGS = '-y'
    command = ' '.join([f'ffmpeg {FFMPEG_ARGS}',
                        f'-i "{input_path}"'])
    ffmpeg = subprocess.run(command,
                            capture_output=True, text=True)
    pattern = re.compile(r'\s*Stream #(\d+):(\d+): Audio:.* (\d+) Hz,')
    for line in ffmpeg.stderr.splitlines():
        m = pattern.match(line)
        if m:
            return int(m[3])
    return None


def convertWavFile(input_path, output_path, from_note, to_note,
                   fs, channels, bit_width):
    """Change the pitch of the sound source file.
    Args:
        input_path: sound source file
        output_path: output WAV file
        from_note: pitch of sound source file
        to_note: target pitch
    """
    # FFMPEG_ARGS = '-hide_banner -loglevel error -y'
    FFMPEG_ARGS = '-y'
    from_freq = int(440 * 2 ** ((from_note - 69) / 12) * 1000 + 0.5)
    to_freq = int(440 * 2 ** ((to_note - 69) / 12) * 1000 + 0.5)
    aresample = f'aresample={fs}'
    asetrate = f'asetrate={fs}*{to_freq}/{from_freq}'
    atempos = []
    while from_freq * 2 < to_freq:
        from_freq *= 2
        atempos.append('atempo=1/2')
    while from_freq > to_freq * 2:
        to_freq *= 2
        atempos.append('atempo=2')
    atempos.append(f'atempo={from_freq}/{to_freq}')
    atempo = ','.join(atempos)
    command = ' '.join([f'ffmpeg {FFMPEG_ARGS}',
                        f'-i "{input_path}"',
                        f'-af {aresample},{asetrate},{atempo}',
                        f'-ar {fs} -ac {channels} -acodec pcm_s{bit_width}le',
                        f'"{output_path}"'])
    output_dir = os.path.dirname(output_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    if not os.path.exists(output_path):
        print(f'-- {command}')
        subprocess.run(command, capture_output=True)


def getOpcodeValue(headers, opcode, default_value=None):
    for h in headers:
        if h is not None:
            for o in h.childNodes:
                if o.nodeType != core.Node.OPCODE_NODE:
                    continue
                if o.nodeName == opcode:
                    return o.textContent
    return default_value


def getKeyOpcodes(headers):
    """Get lokey, hikey and pitch_keycenter from SFZ headers
    Args:
        headers: Search target headers.

    Returns:
        (lokey, hikey, pitch_keycenter)
    """
    lokey = '0'
    hikey = '127'
    pitch_keycenter = '60'
    for h in headers:
        if h is None:
            continue
        for o in h.childNodes:
            if o.nodeType != core.Node.OPCODE_NODE:
                continue
            if o.nodeName == 'key':
                lokey = o.textContent
                hikey = o.textContent
                pitch_keycenter = o.textContent
            if o.nodeName == 'lokey':
                lokey = o.textContent
            if o.nodeName == 'hikey':
                hikey = o.textContent
            if o.nodeName == 'pitch_keycenter':
                pitch_keycenter = o.textContent
    return lokey, hikey, pitch_keycenter


def getOutputWavPath(sample, from_key, to_key):
    """Build path of output WAV file
    """
    head, tail = os.path.split(sample)
    prefix = f'{from_key:03d}{to_key:03d}_'
    output_file = os.path.join(head, prefix + tail)
    output_wav = os.path.splitext(output_file)[0] + '.wav'
    return output_wav


def parseRegion(context, region):
    OUT_FS = 48000
    OUT_CHANNELS = 2
    OUT_BIT_WIDTH = 16

    headers = [context['global'], context['group'], region]

    lokey, hikey, pitch_keycenter = getKeyOpcodes(headers)
    lokey = convertNoteToInt(lokey)
    hikey = convertNoteToInt(hikey)
    pitch_keycenter = convertNoteToInt(pitch_keycenter)

    parent_dir = os.path.dirname(context['path'])
    output_dir = context.get('output_dir', '')
    default_path = context.get('default_path', '')
    sample = getOpcodeValue(headers, 'sample')

    for key in range(lokey, hikey + 1):
        input_path = os.path.join(parent_dir, default_path + sample)
        wav_path = getOutputWavPath(sample, pitch_keycenter, key)
        output_path = os.path.join(output_dir, default_path + wav_path)
        convertWavFile(input_path, output_path, pitch_keycenter, key,
                       OUT_FS, OUT_CHANNELS, OUT_BIT_WIDTH)
        fs = getAudioFs(input_path)
        new_region = region.cloneNode()
        for o in new_region.childNodes:
            if o.nodeType != core.Node.OPCODE_NODE:
                continue
            if o.nodeName in ['key', 'lokey', 'hikey', 'pitch_keycenter']:
                o.textContent = str(key)
            if o.nodeName in ['sample']:
                o.textContent = wav_path.replace('\\', '/')
            if o.nodeName in ['offset', 'end',
                              'loop_start', 'loopstart',
                              'loop_end', 'loopend']:
                if o.textContent != "-1":
                    sa = int(o.textContent)
                    o.textContent = str(int(sa * OUT_FS / fs))
        region.parentNode.insertBefore(new_region, region)
    region.parentNode.removeChild(region)


def parseSfzNode(context, node):
    if node.nodeType == core.Node.HEADER_NODE:
        if node.nodeName == 'global':
            context['global'] = node
        elif node.nodeName == 'group':
            context['group'] = node
        elif node.nodeName == 'region':
            parseRegion(context, node)
        elif node.nodeName == 'control':
            for child in node.childNodes:
                if child.nodeName == 'default_path':
                    context['default_path'] = child.textContent
    else:
        child = node.firstChild
        while child is not None:
            next = child.nextSibling
            parseSfzNode(context, child)
            child = next


def parseSfzFile(path, node, output_dir):
    parseSfzNode({
        'path': path,
        'output_dir': output_dir,
        'global': node
    }, node)
    output_path = os.path.join(output_dir, os.path.basename(path))
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    with open(output_path, mode='w', encoding='utf-8') as f:
        f.write(str(node))


if __name__ == '__main__':
    for arg in args.filename:
        print(f'- {arg}')
        document = core.parse(arg)
        d = document.cloneNode()
        parseSfzFile(arg, d, args.out)
