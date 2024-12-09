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

default: client-server

####################################################################
############### Set up Mininet and Controller ######################
####################################################################

SCRIPTS = ../scripts

export name ?=

MAKEFLAGS += --no-print-directory

.PHONY: mininet controller cli netcfg host-h1 host-h2 tests

help: 
	@echo "Example usage ...\n"
	@echo "- Start Mininet: make mininet\n"
	@echo "- Install Mininet Prereqs/Dependencies: make mininet-prereqs\n"
	@echo "- Start Controller: make controller\n"
	@echo "- Controller CLI: make cli (password is rocks)\n"
	@echo "- Connect Controller to Mininet: make netcfg\n"
	@echo "- Compile Server/Client Binaries: make client-server\n"
	@echo "- Run Tests: make tests\n"
	@echo "- Access Host: make host name=h1\n"
	@echo "- Clean All: make clean\n"

mininet:
	$(SCRIPTS)/mn-stratum --topo linear,2

mininet-prereqs:
	docker exec -it mn-stratum bash -c \
		"sed -i s/deb.debian.org/archive.debian.org/g /etc/apt/sources.list ; \
		 sed -i '$d' /etc/apt/sources.list ; \
		 apt-get update ; \
		 apt-get -y --allow-unauthenticated install iptables python-scapy"

	$(SCRIPTS)/utils/mn-stratum/exec-script h1 \
		"iptables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP"
	$(SCRIPTS)/utils/mn-stratum/exec-script h2 \
		"iptables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP"

controller:
	ONOS_APPS=gui,proxyarp,drivers.bmv2,lldpprovider,hostprovider,fwd \
	$(SCRIPTS)/onos

cli:
	$(SCRIPTS)/onos-cli

netcfg:
	$(SCRIPTS)/onos-netcfg cfg/netcfg.json

# Usage: make host name=h1
host:
	$(SCRIPTS)/utils/mn-stratum/exec $(name)


####################################################################
###################### Compile C programs ##########################
####################################################################

GCC = gcc:4.9
SRCS = srcs

client-server: client server

client:
	docker run --rm -v ./:/workdir -w /workdir $(GCC) \
		gcc -o $(SRCS)/client $(SRCS)/client.c

server:
	docker run --rm -v ./:/workdir -w /workdir $(GCC) \
		gcc -o $(SRCS)/server $(SRCS)/server.c

####################################################################
###3###################### Run tests ###############################
####################################################################

tests:
	make -f Tests.mak all-tests


clean:
	rm -f $(SRCS)/server $(SRCS)/client
	rm -rf .workspace
