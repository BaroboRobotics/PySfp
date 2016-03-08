try:
    import _sfp
except:
    from sfp import _sfp

import threading

import sys
rec_version = (3, 5)
if sys.version_info >= rec_version:
    from . import asyncio

from . import client

class Context():
    def __init__(self):
        self._ctx = _sfp.new_context()
        self._lock = threading.Lock()
        self.set_lock_callback(self.lock)
        self.set_unlock_callback(self.unlock)

    def connect(self):
        _sfp.connect(self._ctx)

    def set_deliver_callback(self, cb):
        '''
        The deliver callback.

        This callback is invoked when a whole packet has been received by SFP.
        The signature of the callback should be:
        cb([byte], length)
        '''
        _sfp.set_deliver_callback(self._ctx, cb)

    def set_write_callback(self, cb):
        '''
        The write callback must be of the following format:
        cb([byte]) -> int
        '''
        _sfp.set_write_callback(self._ctx, cb)

    def write(self, bytestring):
        _sfp.write_packet(self._ctx, bytestring)

    def set_lock_callback(self, cb):
        _sfp.set_lock_callback(self._ctx, cb)

    def set_unlock_callback(self, cb):
        _sfp.set_unlock_callback(self._ctx, cb)

    def deliver(self, byte):
        return _sfp.deliver(self._ctx, byte)

    def lock(self):
        self._lock.acquire()

    def unlock(self):
        self._lock.release()

