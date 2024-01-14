import asyncio
from invoke import task
from storage.tcp_server import main as tcp_server_main


@task
def tcp_server(ctx):
    """Start TCP server"""
    asyncio.run(tcp_server_main())
