import asyncio

from invoke import task

from tcp_server import main


@task
def tcp_writer(ctx):
    """TCP Writer"""
    asyncio.run(main())
