# TODO BEFORE HANDING IN THIS GODDAMN PROJECT

- [x] Realloc
- [x] Lógica de reaproveitar espaços da small e tiny zones
- [x] If there is an error, the malloc() et realloc() functions return a NULL pointer.
- [x] verificar se vai dar pau quando estourar o tamanho das zonas (por conta daquele 1 byte de true/false)
- [x] write tests for your own and also to help evaluators
- [x] e se passar -1 pro malloc


## from pdf
- [x] (Refactor) You are allowed a global variable to manage your allocations and one for the thread-safe
- [x] Your project must be clean code even without norm if it’s ugly you will get 0.
- [x] You must use the mmap(2) and munmap(2) syscall to claim and return the memory zones to the system.
- [x] Each zone must contain at least 100 allocations

## Fix
- [x] jogar metadados pra página dos ledgers
- [x] edge case quando o reuso não é possível mas ainda tem alocação mais pra frente
- [x] 1024 x 1024
- [x] realloc kakakakak
- [x] show_alloc_mem comendo entradas
- [x] separar as funções em arquivos (free malloc etc) - organização repo
- [x] [bonus] hexdump
- [x] [bonus] mutexes 👀
- [x] organizar arquivos dentro da pasta
- [ ] refatorar free pra ficar bonito vai