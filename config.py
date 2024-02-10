from functools import lru_cache

from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    QUEUE_NAME: str = "queue"
    STATUS_NAME: str = "status"
    MAX_QUEUE_SIZE: int = 500_000
    WRITE_BATCH_SIZE: int = 10000
    PIPE_SIZE: int = 100

    REDIS_HOST: str = "localhost"
    REDIS_PORT: int = 6379
    REDIS_DB: int = 0

    SPI_SPEED: int = 10_000_000

    TCP_SERVER_PORT: int = 2345

    # Serial
    TRACE_SERIAL_PORT: str = "/dev/ttyS0"
    TRACE_SERIAL_BAUDRATE: int = 115200
    TRANSMIT_SERIAL_PORT: str = "/dev/ttyS4"
    TRANSMIT_SERIAL_BAUDRATE: int = 1500000

    RECEIVE_PACKET_NUM: int = 8

    model_config = SettingsConfigDict(
        extra="allow",
        env_file=("project.env", ".env"),
    )


@lru_cache()
def get_settings() -> Settings:
    return Settings()
