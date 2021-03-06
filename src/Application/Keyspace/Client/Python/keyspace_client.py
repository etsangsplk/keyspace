# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.31
#
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _keyspace_client
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


class PySwigIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PySwigIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PySwigIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _keyspace_client.delete_PySwigIterator
    __del__ = lambda self : None;
    def value(*args): return _keyspace_client.PySwigIterator_value(*args)
    def incr(*args): return _keyspace_client.PySwigIterator_incr(*args)
    def decr(*args): return _keyspace_client.PySwigIterator_decr(*args)
    def distance(*args): return _keyspace_client.PySwigIterator_distance(*args)
    def equal(*args): return _keyspace_client.PySwigIterator_equal(*args)
    def copy(*args): return _keyspace_client.PySwigIterator_copy(*args)
    def next(*args): return _keyspace_client.PySwigIterator_next(*args)
    def previous(*args): return _keyspace_client.PySwigIterator_previous(*args)
    def advance(*args): return _keyspace_client.PySwigIterator_advance(*args)
    def __eq__(*args): return _keyspace_client.PySwigIterator___eq__(*args)
    def __ne__(*args): return _keyspace_client.PySwigIterator___ne__(*args)
    def __iadd__(*args): return _keyspace_client.PySwigIterator___iadd__(*args)
    def __isub__(*args): return _keyspace_client.PySwigIterator___isub__(*args)
    def __add__(*args): return _keyspace_client.PySwigIterator___add__(*args)
    def __sub__(*args): return _keyspace_client.PySwigIterator___sub__(*args)
    def __iter__(self): return self
PySwigIterator_swigregister = _keyspace_client.PySwigIterator_swigregister
PySwigIterator_swigregister(PySwigIterator)

class imaxdiv_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, imaxdiv_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, imaxdiv_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["quot"] = _keyspace_client.imaxdiv_t_quot_set
    __swig_getmethods__["quot"] = _keyspace_client.imaxdiv_t_quot_get
    if _newclass:quot = _swig_property(_keyspace_client.imaxdiv_t_quot_get, _keyspace_client.imaxdiv_t_quot_set)
    __swig_setmethods__["rem"] = _keyspace_client.imaxdiv_t_rem_set
    __swig_getmethods__["rem"] = _keyspace_client.imaxdiv_t_rem_get
    if _newclass:rem = _swig_property(_keyspace_client.imaxdiv_t_rem_get, _keyspace_client.imaxdiv_t_rem_set)
    def __init__(self, *args): 
        this = _keyspace_client.new_imaxdiv_t(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _keyspace_client.delete_imaxdiv_t
    __del__ = lambda self : None;
imaxdiv_t_swigregister = _keyspace_client.imaxdiv_t_swigregister
imaxdiv_t_swigregister(imaxdiv_t)

imaxabs = _keyspace_client.imaxabs
imaxdiv = _keyspace_client.imaxdiv
strtoimax = _keyspace_client.strtoimax
strtoumax = _keyspace_client.strtoumax
wcstoimax = _keyspace_client.wcstoimax
wcstoumax = _keyspace_client.wcstoumax
class Keyspace_NodeParams(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Keyspace_NodeParams, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Keyspace_NodeParams, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _keyspace_client.new_Keyspace_NodeParams(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _keyspace_client.delete_Keyspace_NodeParams
    __del__ = lambda self : None;
    def Close(*args): return _keyspace_client.Keyspace_NodeParams_Close(*args)
    def AddNode(*args): return _keyspace_client.Keyspace_NodeParams_AddNode(*args)
    __swig_setmethods__["nodec"] = _keyspace_client.Keyspace_NodeParams_nodec_set
    __swig_getmethods__["nodec"] = _keyspace_client.Keyspace_NodeParams_nodec_get
    if _newclass:nodec = _swig_property(_keyspace_client.Keyspace_NodeParams_nodec_get, _keyspace_client.Keyspace_NodeParams_nodec_set)
    __swig_setmethods__["nodes"] = _keyspace_client.Keyspace_NodeParams_nodes_set
    __swig_getmethods__["nodes"] = _keyspace_client.Keyspace_NodeParams_nodes_get
    if _newclass:nodes = _swig_property(_keyspace_client.Keyspace_NodeParams_nodes_get, _keyspace_client.Keyspace_NodeParams_nodes_set)
    __swig_setmethods__["num"] = _keyspace_client.Keyspace_NodeParams_num_set
    __swig_getmethods__["num"] = _keyspace_client.Keyspace_NodeParams_num_get
    if _newclass:num = _swig_property(_keyspace_client.Keyspace_NodeParams_num_get, _keyspace_client.Keyspace_NodeParams_num_set)
Keyspace_NodeParams_swigregister = _keyspace_client.Keyspace_NodeParams_swigregister
Keyspace_NodeParams_swigregister(Keyspace_NodeParams)

Keyspace_ResultBegin = _keyspace_client.Keyspace_ResultBegin
Keyspace_ResultNext = _keyspace_client.Keyspace_ResultNext
Keyspace_ResultIsEnd = _keyspace_client.Keyspace_ResultIsEnd
Keyspace_ResultClose = _keyspace_client.Keyspace_ResultClose
Keyspace_ResultKey = _keyspace_client.Keyspace_ResultKey
Keyspace_ResultValue = _keyspace_client.Keyspace_ResultValue
Keyspace_ResultTransportStatus = _keyspace_client.Keyspace_ResultTransportStatus
Keyspace_ResultConnectivityStatus = _keyspace_client.Keyspace_ResultConnectivityStatus
Keyspace_ResultTimeoutStatus = _keyspace_client.Keyspace_ResultTimeoutStatus
Keyspace_ResultCommandStatus = _keyspace_client.Keyspace_ResultCommandStatus
Keyspace_Create = _keyspace_client.Keyspace_Create
Keyspace_Init = _keyspace_client.Keyspace_Init
Keyspace_Destroy = _keyspace_client.Keyspace_Destroy
Keyspace_GetResult = _keyspace_client.Keyspace_GetResult
Keyspace_SetGlobalTimeout = _keyspace_client.Keyspace_SetGlobalTimeout
Keyspace_SetMasterTimeout = _keyspace_client.Keyspace_SetMasterTimeout
Keyspace_GetGlobalTimeout = _keyspace_client.Keyspace_GetGlobalTimeout
Keyspace_GetMasterTimeout = _keyspace_client.Keyspace_GetMasterTimeout
Keyspace_GetMaster = _keyspace_client.Keyspace_GetMaster
Keyspace_DistributeDirty = _keyspace_client.Keyspace_DistributeDirty
Keyspace_Get = _keyspace_client.Keyspace_Get
Keyspace_DirtyGet = _keyspace_client.Keyspace_DirtyGet
Keyspace_Count = _keyspace_client.Keyspace_Count
Keyspace_CountStr = _keyspace_client.Keyspace_CountStr
Keyspace_DirtyCount = _keyspace_client.Keyspace_DirtyCount
Keyspace_DirtyCountStr = _keyspace_client.Keyspace_DirtyCountStr
Keyspace_ListKeys = _keyspace_client.Keyspace_ListKeys
Keyspace_ListKeysStr = _keyspace_client.Keyspace_ListKeysStr
Keyspace_DirtyListKeys = _keyspace_client.Keyspace_DirtyListKeys
Keyspace_DirtyListKeysStr = _keyspace_client.Keyspace_DirtyListKeysStr
Keyspace_ListKeyValues = _keyspace_client.Keyspace_ListKeyValues
Keyspace_ListKeyValuesStr = _keyspace_client.Keyspace_ListKeyValuesStr
Keyspace_DirtyListKeyValues = _keyspace_client.Keyspace_DirtyListKeyValues
Keyspace_DirtyListKeyValuesStr = _keyspace_client.Keyspace_DirtyListKeyValuesStr
Keyspace_Set = _keyspace_client.Keyspace_Set
Keyspace_TestAndSet = _keyspace_client.Keyspace_TestAndSet
Keyspace_Add = _keyspace_client.Keyspace_Add
Keyspace_AddStr = _keyspace_client.Keyspace_AddStr
Keyspace_Delete = _keyspace_client.Keyspace_Delete
Keyspace_Remove = _keyspace_client.Keyspace_Remove
Keyspace_Rename = _keyspace_client.Keyspace_Rename
Keyspace_Prune = _keyspace_client.Keyspace_Prune
Keyspace_SetExpiry = _keyspace_client.Keyspace_SetExpiry
Keyspace_RemoveExpiry = _keyspace_client.Keyspace_RemoveExpiry
Keyspace_ClearExpiries = _keyspace_client.Keyspace_ClearExpiries
Keyspace_Begin = _keyspace_client.Keyspace_Begin
Keyspace_Submit = _keyspace_client.Keyspace_Submit
Keyspace_Cancel = _keyspace_client.Keyspace_Cancel
Keyspace_IsBatched = _keyspace_client.Keyspace_IsBatched
Keyspace_SetTrace = _keyspace_client.Keyspace_SetTrace


