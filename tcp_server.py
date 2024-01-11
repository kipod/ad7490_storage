import asyncio

from config import Settings, get_settings
from logger import log

SETTINGS: Settings = get_settings()


async def handle_client(reader, writer):
    ...  # handle client here


async def main():
    server = await asyncio.start_server(handle_client, "0.0.0.0", SETTINGS.TCP_SERVER_PORT)

    addr = server.sockets[0].getsockname()
    # print(f'Server started on {addr}')
    log(log.INFO, "Server started on [%s]", addr)

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
