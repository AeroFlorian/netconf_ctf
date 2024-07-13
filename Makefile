VERSION="0.1"

.PHONY: challenges cli all

challenges:
	docker build --file deploy/challenges/Dockerfile --label version=$(VERSION) --tag ctf_netconf_oran:v$(VERSION) .

cli:
	docker build --file deploy/cli/Dockerfile --label version=$(VERSION) --tag cli_ctf:v$(VERSION) .

all: challenges cli

launch_compose:
	docker-compose -f deploy/compose/docker-compose.yml up -d

stop_compose:
	docker-compose -f deploy/compose/docker-compose.yml down

enter_cli_container:
	docker exec -it cli_ctf /bin/bash

connect_cli:
	docker exec -it cli_ctf "./connect_to_server.py"