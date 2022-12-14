# SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
#
# Copyright 2022 Sony Semiconductor Solutions Corporation
#

import os
import re


class Node:
    HEADER_NODE = 1
    OPCODE_NODE = 2
    TEXT_NODE = 3
    COMMENT_NODE = 4
    ROOT_NODE = 5
    MACRO_NODE = 6

    def __init__(self):
        self._parentNode = None
        self._children = []

    @property
    def nodeType(self):
        pass

    @property
    def parentNode(self):
        return self._parentNode

    @parentNode.setter
    def parentNode(self, parentNode):
        self._parentNode = parentNode

    @property
    def nextSibling(self):
        if self.parentNode is not None:
            siblings = self.parentNode._children
            index = siblings.index(self)
            if index + 1 < len(siblings):
                return siblings[index + 1]
        return None

    @property
    def childNodes(self):
        return self._children

    @property
    def firstChild(self):
        if len(self._children) > 0:
            return self._children[0]
        return None

    @property
    def lastChild(self):
        if len(self._children) > 0:
            return self._children[-1]
        return None

    @property
    def nodeName(self):
        return ''

    @property
    def textContent(self):
        return None

    def appendChild(self, child):
        if child in self._children:
            self._children.remove(child)
        self._children.append(child)
        child.parentNode = self

    def insertBefore(self, newChild, refChild):
        if refChild in self._children:
            index = self._children.index(refChild)
            self._children[index:index] = [newChild]
            newChild.parentNode = self
        else:
            self.appendChlid(newChild)

    def removeChild(self, child):
        if child in self._children:
            self._children.remove(child)
            child.parentNode = None
            return child
        else:
            return None

    def replaceChild(self, newChild, oldChild):
        if oldChild in self._children:
            index = self._children.index(oldChild)
            self.children[index] = newChild
            oldChild.parentNode = None
            newChild.parentNode = self
            return oldChild

    def cloneNode(self):
        pass


class HeaderNode(Node):
    def __init__(self, header):
        super(HeaderNode, self).__init__()
        self._pads = [header[0:-len(header.lstrip())],
                      header[len(header.rstrip()):]]
        self._header = header.strip()

    def __str__(self):
        return ''.join(['<',
                        self._pads[0],
                        self._header,
                        self._pads[1],
                        '>',
                        ''.join([str(child) for child in self.childNodes])])

    @property
    def nodeType(self):
        return Node.HEADER_NODE

    @property
    def nodeName(self):
        return self._header

    def cloneNode(self):
        clone = HeaderNode(self._header)
        clone._pads = self._pads
        for child in self._children:
            clone.appendChild(child.cloneNode())
        return clone


class OpcodeNode(Node):
    def __init__(self, opcode, value):
        super(OpcodeNode, self).__init__()
        self._pads = [opcode[0:-len(opcode.lstrip())],
                      opcode[len(opcode.rstrip()):],
                      value[0:-len(value.lstrip())],
                      value[len(value.rstrip()):]]
        self._opcode = opcode.strip()
        self._value = value.strip()

    def __str__(self):
        return ''.join([self._pads[0],
                        self._opcode,
                        self._pads[1],
                        '=',
                        self._pads[2],
                        self._value,
                        self._pads[3]])

    @property
    def nodeType(self):
        return Node.OPCODE_NODE

    @property
    def nodeName(self):
        return self._opcode

    @nodeName.setter
    def nodeName(self, nodeName):
        self._opcode = nodeName

    @property
    def textContent(self):
        return self._value

    @textContent.setter
    def textContent(self, text):
        self._value = text

    def cloneNode(self):
        clone = OpcodeNode(self._opcode, self._value)
        clone._pads = self._pads
        return clone


class TextNode(Node):
    def __init__(self, text):
        super(TextNode, self).__init__()
        self._text = text

    def __str__(self):
        return f'{self._text}'

    @property
    def nodeType(self):
        return Node.TEXT_NODE

    @property
    def textContent(self):
        return self._text

    def cloneNode(self):
        clone = TextNode(self._text)
        return clone


class CommentNode(Node):
    def __init__(self, comment):
        super(CommentNode, self).__init__()
        self._comment = comment

    def __str__(self):
        return f'{self._comment}'

    @property
    def nodeType(self):
        return Node.COMMENT_NODE

    @property
    def textContent(self):
        return self._comment

    def cloneNode(self):
        clone = CommentNode(self._comment)
        return clone


class DocumentNode(Node):
    def __init__(self):
        super(DocumentNode, self).__init__()

    def __str__(self):
        return ''.join([str(child) for child in self.childNodes])

    @property
    def nodeType(self):
        return Node.ROOT_NODE

    def cloneNode(self):
        clone = DocumentNode()
        for child in self._children:
            clone.appendChild(child.cloneNode())
        return clone


class MacroNode(Node):
    def __init__(self, name, value):
        super(MacroNode, self).__init__()
        self._name = name
        self._value = value

    def __str__(self):
        return f'{self._name}{self._value}'

    @property
    def nodeType(self):
        return Node.MACRO_NODE

    @property
    def nodeName(self):
        return self._name

    @property
    def textContent(self):
        return self._value

    def cloneNode(self):
        clone = MacroNode(self._name, self._value)
        return clone


class SimpleSfzParser:
    def __init__(self):
        pass

    def startDocument(self):
        # print(f'startDocument()')
        pass

    def endDocument(self):
        # print(f'endDocument()')
        pass

    def startHeader(self, name):
        # print(f'startHeader({repr(name)})')
        pass

    def endHeader(self, name):
        # print(f'endHeader({repr(name)})')
        pass

    def opcode(self, name, value):
        # print(f'opcode({repr(name)}, {repr(value)})')
        pass

    def characters(self, value):
        # print(f'characters({repr(value)})')
        pass

    def comment(self, value):
        # print(f'comment({repr(value)})')
        pass

    def macro(self, name, value):
        # print(f'macro({repr(name)}, {repr(value)})')
        pass


class DomBuilder(SimpleSfzParser):
    def __init__(self):
        super(DomBuilder, self).__init__()
        self._document = None
        self._cursor = None

    def getDocument(self):
        return self._document

    def startDocument(self):
        super().startDocument()
        node = DocumentNode()
        self._document = node
        self._cursor = node

    def endDocument(self):
        super().endDocument()

    def startHeader(self, name):
        super().startHeader(name)
        node = HeaderNode(name)
        self._document.appendChild(node)
        self._cursor = node

    def endHeader(self, name):
        super().endHeader(name)
        self._cursor = self._document

    def opcode(self, name, value):
        super().opcode(name, value)
        node = OpcodeNode(name, value)
        self._cursor.appendChild(node)

    def characters(self, value):
        super().characters(value)
        node = TextNode(value)
        self._cursor.appendChild(node)

    def comment(self, value):
        super().comment(value)
        node = CommentNode(value)
        self._cursor.appendChild(node)

    def macro(self, name, value):
        super().macro(name, value)
        node = MacroNode(name, value)
        self._document.appendChild(node)


class SimpleSfzModelBuilder:
    def __init__(self):
        self.node = Node()


def parse(file, parser=None, encoding='utf-8'):
    if parser is None:
        parser = DomBuilder()
    parse_sub(file, file, parser, encoding, {}, True)
    return parser.getDocument()


def parse_sub(root_path, path, parser, encoding, defs, parsing_root):
    STATE_READY = 0
    STATE_MACRO = 1
    STATE_DEFINE_MACRO = 2
    STATE_INCLUDE_MACRO = 3
    STATE_HEADER = 4
    STATE_OPCODE = 5
    STATE_VALUE = 6
    STATE_COMMENT = 7

    state = STATE_READY
    buf = ''
    macro_name = ''
    header_name = ''
    opcode_name = ''
    prev_ch = ''
    next_ch = None
    last_token_offset = 0

    if parsing_root:
        parser.startDocument()

    with open(path, mode='r', encoding=encoding) as f:
        while True:
            ch = ''
            if next_ch is not None:
                ch, next_ch = next_ch, None
            else:
                ch = f.read(1)
            if ch == '/':
                next_ch = f.read(1)
                if next_ch == '/':
                    ch += next_ch
                    next_ch = None
            buf += ch
            # print(f'debug: {state} {repr(ch)} {repr(buf)}')
            if state == STATE_READY:
                if ch in ['']:
                    pass
                elif ch == '//':
                    state = STATE_COMMENT
                elif ch == '#':
                    state = STATE_MACRO
                elif ch == '<':
                    holder = buf[0:-len(ch)]
                    parser.characters(holder)
                    if header_name != '':
                        parser.endHeader(header_name)
                    state = STATE_HEADER
                    buf = ch
                    header_name = ''
                elif ch == '=':
                    state = STATE_VALUE
                    buf = ''
                elif ch != ' ' and ch.isprintable():
                    state = STATE_OPCODE
            elif state == STATE_MACRO:
                if ch in ['', '\r', '\n', '//']:
                    if ch == '':
                        holder = buf
                    else:
                        holder = buf[0:-len(ch)]
                    parser.macro(macro_name, holder)
                    state = STATE_READY
                    buf = ch
                elif ch.isspace():
                    pattern = re.compile(r'\s*#\s*(\S+)')
                    m = pattern.match(buf)
                    if m:
                        if m[1] == 'define':
                            macro_name = buf
                            state = STATE_DEFINE_MACRO
                            buf = ''
                        elif m[1] == 'include':
                            macro_name = buf
                            state = STATE_INCLUDE_MACRO
                            buf = ''
                        else:
                            state = STATE_READY
                            buf = ''
            elif state == STATE_DEFINE_MACRO:
                if ch in ['', '\r', '\n', '//']:
                    pattern = re.compile(r'\s*(\$\w+)\s+(\S+)')
                    m = pattern.match(buf)
                    if m:
                        defs[apply_defs(m[1], defs)] = apply_defs(m[2], defs)
                        state = STATE_READY
                        buf = buf[len(m[0]):]
                    else:
                        parser.macro(macro_name, holder)
                        state = STATE_READY
                        buf = ch
                elif ch.isspace():
                    holder = buf[0:-len(ch)]
                    pattern = re.compile(r'\s*(\$\w+)\s+(\S+)')
                    m = pattern.match(holder)
                    if m:
                        defs[apply_defs(m[1], defs)] = apply_defs(m[2], defs)
                        state = STATE_READY
                        buf = ''
            elif state == STATE_INCLUDE_MACRO:
                if ch in ['', '\r', '\n', '//']:
                    parser.macro(macro_name, buf[0:-len(ch)])
                    state = STATE_READY
                    buf = ch
                elif ch == '"':
                    pattern = re.compile(r'\s*"([^"]*)"')
                    m = pattern.match(buf)
                    if m:
                        include_path = os.path.join(
                            os.path.dirname(root_path), m[1])
                        parse_sub(root_path, include_path,
                                  parser, encoding, defs, False)
                        state = STATE_READY
                        buf = ''
            elif state == STATE_HEADER:
                if ch in ['', '\r', '\n']:
                    parser.characters(buf)
                elif ch == '<':
                    parser.characters(buf)
                    if header_name != '':
                        parser.endHeader(header_name)
                    header_name = ''
                    state = STATE_HEADER
                    buf = ''
                elif ch == '>':
                    header_pattern = re.compile(r'^\s*<((\s*)(.*?)(\s*))>$')
                    m = header_pattern.match(buf)
                    if m:
                        header_name = m[1]
                        parser.startHeader(header_name)
                    state = STATE_READY
                    buf = ''
            elif state == STATE_OPCODE:
                if ch in ['', '\r', '\n', '//', '<']:
                    holder = buf[0:-len(ch)]
                    parser.characters(holder)
                    state = STATE_READY
                    buf = ''
                    next_ch = ch
                elif ch == '=':
                    opcode_name = buf[0:-len(ch)]
                    state = STATE_VALUE
                    buf = ''
                    last_token_offset = 0
            elif state == STATE_VALUE:
                if ch in ['', '\r', '\n', '//', '<']:
                    if ch == '':
                        holder = buf
                    else:
                        holder = buf[0:-len(ch)]
                    parser.opcode(apply_defs(opcode_name, defs),
                                  apply_defs(holder, defs))
                    state = STATE_READY
                    buf = ''
                    next_ch = ch
                elif ch == '#':
                    if prev_ch.isspace():
                        parser.opcode(apply_defs(opcode_name, defs),
                                      apply_defs(buf[0:-len(ch)], defs))
                        state = STATE_READY
                        buf = ''
                        next_ch = ch
                elif ch == '=':
                    if last_token_offset > 0:
                        value = buf[:last_token_offset]
                        if len(value.strip()) > 0:
                            parser.opcode(apply_defs(opcode_name, defs),
                                          apply_defs(value, defs))
                            opcode_name = buf[last_token_offset:-len(ch)]
                            buf = ''
                            last_token_offset = 0
                if prev_ch.isspace() and not ch.isspace():
                    last_token_offset = len(buf) - 1
            elif state == STATE_COMMENT:
                if ch in ['', '\r', '\n']:
                    parser.comment(buf)
                    state = STATE_READY
                    buf = ''
            prev_ch = ch
            if ch == '':
                break
        if header_name != '':
            if len(buf) > 0:
                parser.characters(buf)
            parser.endHeader(header_name)
        if parsing_root:
            parser.endDocument()


def apply_defs(str, defs):
    result = str[:]
    start = 0
    while result.find('$', start) >= 0:
        lbound = result.index('$')
        ubound = lbound + 1
        while True:
            ch = result[ubound:ubound + 1]
            if ch == '_' or ch.isalnum():
                ubound += 1
                continue
            break
        key = result[lbound:ubound]
        if key in defs:
            result = result[0:lbound] + defs[key] + result[ubound:]
        else:
            start += 1
    return result
