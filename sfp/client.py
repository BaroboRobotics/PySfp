from .asyncio import SfpProtocol
import asyncio
import functools

__all__ = ['connect']

@asyncio.coroutine
def connect(host, port, loop=None, klass=SfpProtocol):
    ''' Connect to an SFP server.

    :param host: The remote hostname or IP address to connect to
    :param port: The port to connect to
    :type port: int,str
    :rtype: (transport, protocol)
    '''
    if not loop:
        loop = asyncio.get_event_loop()

    transport, protocol = yield from loop.create_connection(
            functools.partial(klass, loop), host=host, port=port)
    return (transport, protocol)
