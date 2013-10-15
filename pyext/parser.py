
import .yyast
import mmap
import struct

def parse_node(m):
    (
        node_name,
        node_size,
        line_nr,
        column_nr,
        file_nr,
        reserved1,
        reserved2,
        node_type
    ) = struct.unpack(">8sQLLLHBB", m[:32])

    if line_nr   == 0xffffffff: line_nr   = None
    if column_nr == 0xffffffff: column_nr = None
    if file_nr   == 0xffffffff: file_nr   = None

    node_info = {
        "node_name": node_name,
        "node_size": node_size,
        "line_nr": line_nr,
        "column_nr": column_nr,
        "file_nr": file_nr,
        "node_type": node_type,
    }

    base_class = base_classes[node_type]
    class_name = "Node_" + ("__at_" + node_name[1:] if node_name[0] == "@" else node_name)

def parse(fd):
    m = mmap.mmap(fd.fileno(), 0)
    return parse_node(m)

