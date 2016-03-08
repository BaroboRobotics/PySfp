from .asyncio import SfpProtocol
import asyncio

async def connect(host, port, loop=None, klass=SfpProtocol):
    ''' Connect to an SFP server.

    :param host: The remote hostname or IP address to connect to
    :param port: The port to connect to
    :type port: int,str
    :rtype: (transport, protocol)
    '''
    if not loop:
        loop = asyncio.get_event_loop()

    transport, protocol = await loop.create_connection(
            klass, host=host, port=port)
    return (transport, protocol)
