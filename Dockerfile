FROM python:3.11-slim

WORKDIR /app

RUN pip install poetry

COPY poetry.lock .
COPY pyproject.toml .

RUN POETRY_VIRTUALENVS_CREATE=false poetry install --no-dev --no-interaction --no-ansi

EXPOSE 80

COPY . .

CMD ["ls", "-la"]
