# Copyright (c) 2011-2013, Take Vos
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright notice, 
#   this list of conditions and the following disclaimer in the documentation 
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 

import yyast
import mmap
import struct
import NodeInfo

def strip_null(x):
    """Strip null bytes at the end of a string that is located in memory.
    """
    for i in range(len(x) -1, -1, -1):
        if x[i] != '\0':
            break
    return x[0:i+1]

class YYASTParserException (Exception):
    def __init__(self, message):
        Exeption.__init__(self, args=(message,))

class Parser (object):
    def __init__(self):
        self.classes = {}
        self.subparsers = {
            yyast.NODE_TYPE_NULL:               self.parse_null_node,
            yyast.NODE_TYPE_LEAF:               self.parse_leaf_node,
            yyast.NODE_TYPE_BRANCH:             self.parse_branch_node,
            yyast.NODE_TYPE_TEXT:               self.parse_text_node,
            yyast.NODE_TYPE_POSITIVE_INTEGER:   self.parse_positive_integer_node,
            yyast.NODE_TYPE_NEGATIVE_INTEGER:   self.parse_negative_integer_node,
            yyast.NODE_TYPE_BINARY_FLOAT:       self.parse_binary_float_node,
            yyast.NODE_TYPE_DECIMAL_FLOAT:      self.parse_decimal_float_node,
            yyast.NODE_TYPE_LIST:               self.parse_list_node,
            yyast.NODE_TYPE_COUNT:              self.parse_count_node,
        }

    def register_factory(self, node_name, factory):
        """Register a class for parsing a node.

        @param node_name    The name of the node in the file to be parsed.
        @param factory      The class that should be used as a factory for this node.
        """
        self.factories[node_name] = factory

    def parse_null_node(self, factory, node_info, internal_data):
        if len(internal_data) > 0:
            raise YYASTParserException("Null node should not have data.")

        node = factory(node_info)
        node.parsing_done()
        return node

    def parse_leaf_node(self, factory, node_info, internal_data):
        if len(internal_data) > 0:
            raise YYASTParserException("Leaf node should not have data.")

        node = factory(node_info)
        node.parsing_done()
        return node

    def parse_branch_node(self, factory, node_info, internal_data):
        node = factory(node_info)

        # Parse the child nodes and add it to the node.
        while internal_data:
            child_node, internal_data = self.parse_node(internal_data)
            node.add_child(child_node)
      
        node.parsing_done()   
        return node

    def parse_text_node(self, factory, node_info, internal_data):
        internal_data = strip_null(internal_data)
        value = internal_data.decode("UTF-8")
        node = factory(node_info, value)
        node.parsing_done()
        return node

    def parse_positive_integer_node(self, factory, node_info, internal_data):
        if len(internal_data) != 8:
            raise YYASTParserException("Positive integer node should be exactly 64 bit.")

        (value,) = struct.unpack(">Q", internal_data)
        node = factory(node_info, value)
        node.parsing_done()
        return node

    def parse_negative_integer_node(self, factory, node_info, internal_data):
        if len(internal_data) != 8:
            raise YYASTParserException("Negative integer node should be exactly 64 bit.")

        (value,) = struct.unpack(">Q", internal_data)
        node = factory(node_info, -value)
        node.parsing_done()
        return node

    def parse_binary_float_node(self, factory, node_info, internal_data):
        if len(internal_data) != 8:
            raise YYASTParserException("Binary float node should be exactly 64 bit.")

        (value,) = struct.unpack(">d", internal_data)
        node = factory(node_info, value)
        node.parsing_done()
        return node

    def parse_decimal_float_node(self, factory, node_info, internal_data):
        if len(internal_data) != 8:
            raise YYASTParserException("Decimal float node should not have data.")

        raise NotImplementedError("Decimal float is not implemented in the parser.")

    def parse_list_node(self, factory, node_info, internal_data):
        raise NotImplementedError("List node should not exist in the file.")

    def parse_count_node(self, factory, node_info, internal_data):
        raise NotImplementedError("Count node should not exist in the file.")

    def parse_node(self, data):
        """Parse a single node from the data

        This function will parse the data and recursively create and return nodes.

        @param data Data from a yyast file.
        @return node, rest  The parsed node, the rest data that is left after parsing the node.
        """

        # Parse the header of the data.
        (
            node_name,
            node_size,
            line_nr,
            column_nr,
            file_nr,
            reserved1,
            reserved2,
            node_type
        ) = struct.unpack(">8sQLLLHBB", data[:32])

        # The internal data for the node starts from the header, until the length of the node data.
        internal_data = data[32:node_size]

        # The rest of the data after the node.
        rest_data = data[node_size:]

        # Convert fields into more usable values.
        if line_nr   == 0xffffffff: line_nr   = None
        if column_nr == 0xffffffff: column_nr = None
        if file_nr   == 0xffffffff: file_nr   = None
        node_name = node_name.strip()

        node_info = NodeInfo(
            node_type=node_type,
            node_name=node_name,
            node_size=node_size,
            line_nr=line_nr,
            column_nr=column_nr,
            file_nr=file_nr
        )

        # Get the factory class for the node.
        try:
            factory = self.factories[node_name]
        except KeyError:
            raise YYASTParserException("Could not find factory for '%s' node" % node_name)

        try:
            subparser = self.subparsers[node_type]
        except KeyError:
            raise YYASTParserException("Unknown node type %i." % node_type)
        
        node = subparser(factory, node_info, internal_data)
        return node, rest_data


    def parse(self, fd):
        m = mmap.mmap(fd.fileno(), 0, access=mmap.ACCESS_READ)
        node, rest_data = self.parse_node(m)

        if len(rest_data) != 0:
            raise YYASTParserException("More data at end of file")

        return node

