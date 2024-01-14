from invoke import task
from storage.writer import main as writer_main


@task
def writer(ctx):
    """Start writer"""
    writer_main()
