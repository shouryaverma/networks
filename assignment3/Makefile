############################################################################
##
##     This file is part of Purdue CS 536.
##
##     Purdue CS 536 is free software: you can redistribute it and/or modify
##     it under the terms of the GNU General Public License as published by
##     the Free Software Foundation, either version 3 of the License, or
##     (at your option) any later version.
##
##     Purdue CS 536 is distributed in the hope that it will be useful,
##     but WITHOUT ANY WARRANTY; without even the implied warranty of
##     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##     GNU General Public License for more details.
##
##     You should have received a copy of the GNU General Public License
##     along with Purdue CS 536. If not, see <https://www.gnu.org/licenses/>.
##
#############################################################################

PYTHON2_OK := $(shell python2 --version 2>&1)
PYTHON3_OK := $(shell python3 --version 2>&1)
ifeq ('$(PYTHON2_OK)','')
	ifeq ('$(PYTHON3_OK)','')
		$(error package 'python 2 or 3' not found)
	else
		PYTHON = python3
	endif
else
	PYTHON = python2
endif

SCRIPTS = ../scripts

export MN_STRATUM_IMG = opennetworking/mn-stratum
export P4RUNTIME_SH_IMG = p4lang/p4runtime-sh:latest

export P4_PROGRAM_DIRNAME ?=
export P4_PROGRAM_NAME ?=
export P4RT_PROGRAM_DIRNAME ?=
export P4RT_PROGRAM_NAME ?=

export topo ?= linear,2,2
export name ?=
export grpc_port ?= 50001

.PHONY: help mininet enable-vlan disable-vlan controller controller-logs bridge switch host clean

help: 
	@echo "Example usage ...\n"
	@echo "- Start Mininet: make mininet topo=linear,2,2\n"
	@echo "- Enable VLAN: make enable-vlan topo=linear,2,2\n"
	@echo "- Disable VLAN: make disable-vlan topo=linear,2,2\n"
	@echo "- Start Controller: make controller name=bridge grpc_port=50001 topo=linear,2,2\n"
	@echo "- Log Controller Stats: make controller-logs name=bridge grpc_port=50001\n"
	@echo "- Access Host: make host name=h1s1\n"
	@echo "- Clean All: make clean\n"

# Usage: make mininet topo=linear,2,2
mininet:
	$(SCRIPTS)/mn-stratum --topo $(topo)
	make clean

# Usage: make enable-vlan topo=linear,2,2
enable-vlan: .mininet-install-prereqs .mininet-enable-vlan

# Usage: make disable-vlan topo=linear,2,2
disable-vlan: .mininet-disable-vlan

# Usage: make controller name=bridge grpc_port=50001 topo=linear,2,2
controller:
	make .controller-$(name)

# Usage: make controller-logs name=bridge grpc_port=50001
controller-logs:
	make .controller-$(name)-logs

# Usage: make host name=h1s1
host:
	$(SCRIPTS)/utils/mn-stratum/exec $(name)

clean: .p4rt-clean .p4-clean

####################################################################
# Controller Types
####################################################################

.controller-bridge-sh:
	P4_PROGRAM_NAME=bridge \
	P4RT_PROGRAM_NAME=bridge \
	make .p4rt-sh

.controller-bridge:
	P4_PROGRAM_NAME=bridge \
	P4RT_PROGRAM_NAME=bridge \
	make .p4rt-script

.controller-bridge-logs:
	P4_PROGRAM_NAME=bridge \
	make .p4rt-logs

.controller-switch-sh:
	P4_PROGRAM_NAME=switch \
	P4RT_PROGRAM_NAME=switch \
	make .p4rt-sh

.controller-switch:
	P4_PROGRAM_NAME=switch \
	P4RT_PROGRAM_NAME=switch \
	make .p4rt-script

.controller-switch-logs:
	P4_PROGRAM_NAME=switch \
	make .p4rt-logs

####################################################################
# P4 Runtime 
####################################################################

.p4rt-sh: .p4-build
	mkdir -p logs/$(P4_PROGRAM_DIRNAME)
	P4RUNTIME_SH_DOCKER_NAME=p4runtime-sh-$(grpc_port) \
	$(SCRIPTS)/p4runtime-sh \
		--grpc-addr 127.0.0.1:$(grpc_port) \
  		--device-id 1 --election-id 0,1 \
  		--config cfg/$(P4_PROGRAM_DIRNAME)/$(P4_PROGRAM_NAME)-$(grpc_port)-p4info.txt,cfg/$(P4_PROGRAM_DIRNAME)/$(P4_PROGRAM_NAME)-$(grpc_port).json

.p4rt-script: .p4-build
	mkdir -p logs/$(P4_PROGRAM_DIRNAME)
	P4RUNTIME_SH_DOCKER_NAME=p4runtime-sh-$(grpc_port) \
	$(SCRIPTS)/p4runtime-sh.run-script \
		"p4rt-src/$(P4RT_PROGRAM_DIRNAME)/$(P4RT_PROGRAM_NAME).py --grpc-port=$(grpc_port) --topo-config=topo/$(topo).json"

.p4rt-logs:
	cat logs/$(P4_PROGRAM_DIRNAME)/$(P4_PROGRAM_NAME)-$(grpc_port)-table.json

.p4rt-clean:
	rm -rf logs

####################################################################
# Build P4
####################################################################

.p4-build:
	mkdir -p cfg/$(P4_PROGRAM_DIRNAME)
	$(SCRIPTS)/p4c p4c-bm2-ss --arch v1model \
		-o cfg/$(P4_PROGRAM_DIRNAME)/$(P4_PROGRAM_NAME)-$(grpc_port).json \
		-DTARGET_BMV2 -DCPU_PORT=255 \
		--p4runtime-files cfg/$(P4_PROGRAM_DIRNAME)/$(P4_PROGRAM_NAME)-$(grpc_port)-p4info.txt \
		p4-src/$(P4_PROGRAM_DIRNAME)/$(P4_PROGRAM_NAME).p4
	
.p4-clean:
	rm -rf cfg

####################################################################
# Network Configs 
####################################################################

.mininet-install-prereqs:
	docker exec -it mn-stratum bash -c \
		"apt-get --allow-insecure-repositories --allow-unauthenticated update ; \
		 apt-get -y --allow-unauthenticated install vlan"

.mininet-enable-vlan:
	$(PYTHON) utils/mininet-vlan.py --scripts-dir $(SCRIPTS) --enable --topo-config topo/$(topo).json

.mininet-disable-vlan:
	$(PYTHON) utils/mininet-vlan.py --scripts-dir $(SCRIPTS) --disable --topo-config topo/$(topo).json

