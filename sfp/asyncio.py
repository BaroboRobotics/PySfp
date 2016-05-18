import asyncio
import logging
import sfp

logger = logging.getLogger('sfp.asyncio.protocol')

class SfpProtocol(asyncio.Protocol):
    def __init__(self, asyncio_loop):
        self._context = sfp.Context()
        self._loop = asyncio_loop
        self._q = asyncio.Queue(loop=self._loop)

    @asyncio.coroutine
    def close(self):
        logger.info('Close')
        self._transport.close()

    def connection_made(self, transport):
        self._transport = transport
        self._context.set_write_callback(self._write)
        self._context.set_deliver_callback(self.__deliver)
        self._context.connect()
        logger.info('Connection established')

    def connection_lost(self, exc):
        '''
        This is called when the connection is lost. Override me.
        '''
        logger.info('Remote closed connection: '+str(exc))

    def data_received(self, data):
        logger.info('Received {} bytes from remote host.'.format(len(data)))
        for byte in data:
            plen = self._context.deliver(int(byte))
    
    @asyncio.coroutine
    def recv(self):
        rc = yield from self._q.get()
        return rc

    @asyncio.coroutine
    def send(self, data):
        self._context.write(data)

    def write(self, data):
        self._context.write(data)
        logger.info('Sent {} bytes to remote host.'.format(len(data)))

    def _write(self, data):
        self._transport.write(data)
        return len(data)

    def __deliver(self, bytestring, length):
        '''
        This is a trampoline to self.deliver, since this function will be called
        from C space
        '''
        if length == 0:
            return
        asyncio.run_coroutine_threadsafe(
                self._q.put(bytestring),
                self._loop)

