all:
	@echo 'example'
	@./assembler.py sample/example.s > /dev/null
	@./diff.py example.o sample/example.solution.o -s sample/example.s
	@echo ''
	@echo ''
	@echo 'example2'
	@./assembler.py sample/example2.s > /dev/null
	@./diff.py example2.o sample/example2.solution.o -s sample/example2.s
	@echo ''
	@echo ''
	@echo 'example3'
	@./assembler.py sample/example3.s > /dev/null
	@./diff.py example3.o sample/example3.solution.o -s sample/example3.s
	@echo ''
	@echo ''
	@echo 'example4'
	@./assembler.py sample/example4.s > /dev/null
	@./diff.py example4.o sample/example4.solution.o -s sample/example4.s
	@rm -rf example*
