
import yyast
import mmap
import struct

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

    def parse_null_node(self, factory, node_info, inner_data):
        if len(inner_data) > 0:
            raise YYASTParserException("Null node should not have data.")

        return factory(node_info)

    def parse_leaf_node(self, factory, node_info, inner_data):
        if len(inner_data) > 0:
            raise YYASTParserException("Leaf node should not have data.")

        return factory(node_info)

    def parse_branch_node(self, factory, node_info, inner_data):
        node = factory(node_info)

        # Parse the child nodes and add it to the node.
        while internal_data:
            child_node, internal_data = self.parse_node(internal_data)
            node.add_child(child_node)
        
        return node

    def parse_text_node(self, factory, node_info, inner_data):
        value = inner_data.decode("UTF-8")
        return factory(node_info, value)

    def parse_positive_integer_node(self, factory, node_info, inner_data):
        if len(inner_data) != 8:
            raise YYASTParserException("Positive integer node should be exactly 64 bit.")

        (value,) = struct.unpack(">Q", inner_data)
        return factory(node_info, value)

    def parse_negative_integer_node(self, factory, node_info, inner_data):
        if len(inner_data) != 8:
            raise YYASTParserException("Negative integer node should be exactly 64 bit.")

        (value,) = struct.unpack(">Q", inner_data)
        return factory(node_info, -value)

    def parse_binary_float_node(self, factory, node_info, inner_data):
        if len(inner_data) != 8:
            raise YYASTParserException("Binary float node should be exactly 64 bit.")

        (value,) = struct.unpack(">d", inner_data)
        return factory(node_info, value)

    def parse_decimal_float_node(self, factory, node_info, inner_data):
        if len(inner_data) != 8:
            raise YYASTParserException("Decimal float node should not have data.")

        raise NotImplementedError("Decimal float is not implemented in the parser.")

    def parse_list_node(self, factory, node_info, inner_data):
        raise NotImplementedError("List node should not exist in the file.")

    def parse_count_node(self, factory, node_info, inner_data):
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

        node_info = {
            "node_name":    node_name,
            "node_size":    node_size,
            "line_nr":      line_nr,
            "column_nr":    column_nr,
            "file_nr":      file_nr,
            "node_type":    node_type,
        }

        # Get the factory class for the node.
        factory = self.factories[node_name]

        try:
            subparser = self.subparsers[node_type]
        except KeyError:
            raise YYASTParserException("Unknown node type %i.", node_type)
        
        node = subparser(factory, node_info, inner_data)
        return node, rest_data


    def parse(self, fd):
        m = mmap.mmap(fd.fileno(), 0)
        return parse_node(m)

