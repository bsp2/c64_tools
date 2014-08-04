
all: ../tci/config.tci
	@cat ../tci/config.tci >>tmp.tks
	@cat gen_link_area.tks >>tmp.tks
	@tks tmp.tks
	@rm -f tmp.tks

clean:
	rm -f link_area*.cmd

