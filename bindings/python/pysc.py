# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.7
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_pysc', [dirname(__file__)])
        except ImportError:
            import _pysc
            return _pysc
        if fp is not None:
            try:
                _mod = imp.load_module('_pysc', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _pysc = swig_import_helper()
    del swig_import_helper
else:
    import _pysc
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


BOUND_EMPTY_SLOT_SEARCH = _pysc.BOUND_EMPTY_SLOT_SEARCH
USE_SEGMENT_EMPTY_SLOT_BUFFER = _pysc.USE_SEGMENT_EMPTY_SLOT_BUFFER
USE_NETWORK_SCALE = _pysc.USE_NETWORK_SCALE
USE_TWO_ORIENTED_ARC_LIST = _pysc.USE_TWO_ORIENTED_ARC_LIST
SEGMENT_EMPTY_SEARCH_LEN = _pysc.SEGMENT_EMPTY_SEARCH_LEN
SEGMENT_EMPTY_BUFFER_SIZE = _pysc.SEGMENT_EMPTY_BUFFER_SIZE
SEGMENT_EMPTY_MAX_UPDATE_THREADS = _pysc.SEGMENT_EMPTY_MAX_UPDATE_THREADS
SEGS_QUEUE_SIZE = _pysc.SEGS_QUEUE_SIZE
MAX_PATH_LENGTH = _pysc.MAX_PATH_LENGTH
SC_FALSE = _pysc.SC_FALSE
SC_TRUE = _pysc.SC_TRUE
class _sc_addr(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, _sc_addr, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, _sc_addr, name)
    __repr__ = _swig_repr
    __swig_setmethods__["seg"] = _pysc._sc_addr_seg_set
    __swig_getmethods__["seg"] = _pysc._sc_addr_seg_get
    if _newclass:seg = _swig_property(_pysc._sc_addr_seg_get, _pysc._sc_addr_seg_set)
    __swig_setmethods__["offset"] = _pysc._sc_addr_offset_set
    __swig_getmethods__["offset"] = _pysc._sc_addr_offset_get
    if _newclass:offset = _swig_property(_pysc._sc_addr_offset_get, _pysc._sc_addr_offset_set)
    def __init__(self): 
        this = _pysc.new__sc_addr()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _pysc.delete__sc_addr
    __del__ = lambda self : None;
_sc_addr_swigregister = _pysc._sc_addr_swigregister
_sc_addr_swigregister(_sc_addr)

sc_type_node = _pysc.sc_type_node
sc_type_link = _pysc.sc_type_link
sc_type_edge_common = _pysc.sc_type_edge_common
sc_type_arc_common = _pysc.sc_type_arc_common
sc_type_arc_access = _pysc.sc_type_arc_access
sc_type_const = _pysc.sc_type_const
sc_type_var = _pysc.sc_type_var
sc_type_arc_pos = _pysc.sc_type_arc_pos
sc_type_arc_neg = _pysc.sc_type_arc_neg
sc_type_arc_fuz = _pysc.sc_type_arc_fuz
sc_type_arc_temp = _pysc.sc_type_arc_temp
sc_type_arc_perm = _pysc.sc_type_arc_perm
sc_type_node_tuple = _pysc.sc_type_node_tuple
sc_type_node_struct = _pysc.sc_type_node_struct
sc_type_node_role = _pysc.sc_type_node_role
sc_type_node_norole = _pysc.sc_type_node_norole
sc_type_node_class = _pysc.sc_type_node_class
sc_type_node_abstract = _pysc.sc_type_node_abstract
sc_type_node_material = _pysc.sc_type_node_material
sc_type_arc_pos_const_perm = _pysc.sc_type_arc_pos_const_perm
sc_type_element_mask = _pysc.sc_type_element_mask
sc_type_constancy_mask = _pysc.sc_type_constancy_mask
sc_type_positivity_mask = _pysc.sc_type_positivity_mask
sc_type_permanency_mask = _pysc.sc_type_permanency_mask
sc_type_node_struct_mask = _pysc.sc_type_node_struct_mask
sc_type_arc_mask = _pysc.sc_type_arc_mask
SC_RESULT_ERROR = _pysc.SC_RESULT_ERROR
SC_RESULT_OK = _pysc.SC_RESULT_OK
SC_RESULT_ERROR_INVALID_PARAMS = _pysc.SC_RESULT_ERROR_INVALID_PARAMS
SC_RESULT_ERROR_INVALID_TYPE = _pysc.SC_RESULT_ERROR_INVALID_TYPE
SC_RESULT_ERROR_IO = _pysc.SC_RESULT_ERROR_IO
SC_RESULT_ERROR_INVALID_STATE = _pysc.SC_RESULT_ERROR_INVALID_STATE
SC_MAX_CHECKSUM_LEN = _pysc.SC_MAX_CHECKSUM_LEN
class _sc_check_sum(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, _sc_check_sum, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, _sc_check_sum, name)
    __repr__ = _swig_repr
    __swig_setmethods__["data"] = _pysc._sc_check_sum_data_set
    __swig_getmethods__["data"] = _pysc._sc_check_sum_data_get
    if _newclass:data = _swig_property(_pysc._sc_check_sum_data_get, _pysc._sc_check_sum_data_set)
    __swig_setmethods__["len"] = _pysc._sc_check_sum_len_set
    __swig_getmethods__["len"] = _pysc._sc_check_sum_len_get
    if _newclass:len = _swig_property(_pysc._sc_check_sum_len_get, _pysc._sc_check_sum_len_set)
    def __init__(self): 
        this = _pysc.new__sc_check_sum()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _pysc.delete__sc_check_sum
    __del__ = lambda self : None;
_sc_check_sum_swigregister = _pysc._sc_check_sum_swigregister
_sc_check_sum_swigregister(_sc_check_sum)

SC_EVENT_ADD_OUTPUT_ARC = _pysc.SC_EVENT_ADD_OUTPUT_ARC
SC_EVENT_ADD_INPUT_ARC = _pysc.SC_EVENT_ADD_INPUT_ARC
SC_EVENT_REMOVE_OUTPUT_ARC = _pysc.SC_EVENT_REMOVE_OUTPUT_ARC
SC_EVENT_REMOVE_INPUT_ARC = _pysc.SC_EVENT_REMOVE_INPUT_ARC
SC_EVENT_CHANGE_LINK_CONTENT = _pysc.SC_EVENT_CHANGE_LINK_CONTENT

def sc_memory_initialize(*args):
  return _pysc.sc_memory_initialize(*args)
sc_memory_initialize = _pysc.sc_memory_initialize

def sc_memory_shutdown():
  return _pysc.sc_memory_shutdown()
sc_memory_shutdown = _pysc.sc_memory_shutdown

def sc_memory_is_initialized():
  return _pysc.sc_memory_is_initialized()
sc_memory_is_initialized = _pysc.sc_memory_is_initialized

def sc_memory_is_element(*args):
  return _pysc.sc_memory_is_element(*args)
sc_memory_is_element = _pysc.sc_memory_is_element

def sc_memory_element_free(*args):
  return _pysc.sc_memory_element_free(*args)
sc_memory_element_free = _pysc.sc_memory_element_free

def sc_memory_node_new(*args):
  return _pysc.sc_memory_node_new(*args)
sc_memory_node_new = _pysc.sc_memory_node_new

def sc_memory_link_new():
  return _pysc.sc_memory_link_new()
sc_memory_link_new = _pysc.sc_memory_link_new

def sc_memory_arc_new(*args):
  return _pysc.sc_memory_arc_new(*args)
sc_memory_arc_new = _pysc.sc_memory_arc_new

def sc_memory_get_element_type(*args):
  return _pysc.sc_memory_get_element_type(*args)
sc_memory_get_element_type = _pysc.sc_memory_get_element_type

def sc_memory_get_arc_begin(*args):
  return _pysc.sc_memory_get_arc_begin(*args)
sc_memory_get_arc_begin = _pysc.sc_memory_get_arc_begin

def sc_memory_get_arc_end(*args):
  return _pysc.sc_memory_get_arc_end(*args)
sc_memory_get_arc_end = _pysc.sc_memory_get_arc_end

def sc_memory_set_link_content(*args):
  return _pysc.sc_memory_set_link_content(*args)
sc_memory_set_link_content = _pysc.sc_memory_set_link_content

def sc_memory_find_links_with_content(*args):
  return _pysc.sc_memory_find_links_with_content(*args)
sc_memory_find_links_with_content = _pysc.sc_memory_find_links_with_content

def sc_helper_init():
  return _pysc.sc_helper_init()
sc_helper_init = _pysc.sc_helper_init

def sc_helper_shutdown():
  return _pysc.sc_helper_shutdown()
sc_helper_shutdown = _pysc.sc_helper_shutdown

def sc_helper_find_element_by_system_identifier(*args):
  return _pysc.sc_helper_find_element_by_system_identifier(*args)
sc_helper_find_element_by_system_identifier = _pysc.sc_helper_find_element_by_system_identifier

def sc_helper_set_system_identifier(*args):
  return _pysc.sc_helper_set_system_identifier(*args)
sc_helper_set_system_identifier = _pysc.sc_helper_set_system_identifier

def sc_helper_get_keynode(*args):
  return _pysc.sc_helper_get_keynode(*args)
sc_helper_get_keynode = _pysc.sc_helper_get_keynode
SC_STREAM_READ = _pysc.SC_STREAM_READ
SC_STREAM_WRITE = _pysc.SC_STREAM_WRITE
SC_STREAM_APPEND = _pysc.SC_STREAM_APPEND
SC_STREAM_SEEK = _pysc.SC_STREAM_SEEK
SC_STREAM_TELL = _pysc.SC_STREAM_TELL
SC_STREAM_SEEK_SET = _pysc.SC_STREAM_SEEK_SET
SC_STREAM_SEEK_CUR = _pysc.SC_STREAM_SEEK_CUR
SC_STREAM_SEEK_END = _pysc.SC_STREAM_SEEK_END

def sc_stream_free(*args):
  return _pysc.sc_stream_free(*args)
sc_stream_free = _pysc.sc_stream_free

def sc_stream_read_data(*args):
  return _pysc.sc_stream_read_data(*args)
sc_stream_read_data = _pysc.sc_stream_read_data

def sc_stream_write_data(*args):
  return _pysc.sc_stream_write_data(*args)
sc_stream_write_data = _pysc.sc_stream_write_data

def sc_stream_seek(*args):
  return _pysc.sc_stream_seek(*args)
sc_stream_seek = _pysc.sc_stream_seek

def sc_stream_eof(*args):
  return _pysc.sc_stream_eof(*args)
sc_stream_eof = _pysc.sc_stream_eof

def sc_stream_get_length(*args):
  return _pysc.sc_stream_get_length(*args)
sc_stream_get_length = _pysc.sc_stream_get_length

def sc_stream_get_position(*args):
  return _pysc.sc_stream_get_position(*args)
sc_stream_get_position = _pysc.sc_stream_get_position

def sc_stream_check_flag(*args):
  return _pysc.sc_stream_check_flag(*args)
sc_stream_check_flag = _pysc.sc_stream_check_flag

def sc_stream_file_new(*args):
  return _pysc.sc_stream_file_new(*args)
sc_stream_file_new = _pysc.sc_stream_file_new

def sc_stream_memory_new(*args):
  return _pysc.sc_stream_memory_new(*args)
sc_stream_memory_new = _pysc.sc_stream_memory_new
# This file is compatible with both classic and new-style classes.


