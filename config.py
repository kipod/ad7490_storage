from functools import lru_cache

from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    QUEUE_NAME: str = "queue"
    STATUS_NAME: str = "status"
    MAX_QUEUE_SIZE: int = 10_000
    PIPE_SIZE: int = 200

    REDIS_HOST: str = "localhost"
    REDIS_PORT: int = 6379
    REDIS_DB: int = 0

    model_config = SettingsConfigDict(
        extra="allow",
        env_file=("project.env", ".env"),
    )


@lru_cache()
def get_settings() -> Settings:
    return Settings()
