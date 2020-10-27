#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "include/board.hpp"
#include <iostream>
#include <string>
#include <dlfcn.h>

typedef void* (*f_init_t)(int);
typedef dir_t (*f_gen_move_t)(void*, Board&);
typedef void (*f_destroy_t)(void*);

class Controller {
public:
    Controller()
        : _init(0)
        , _gen_move(0)
        , _destroy(0)
        , _ctx(0)
        , _handle(0) {}

    Controller(const std::string& path, int lvl)
        : Controller() {
        init(path, lvl);
    }

    ~Controller() {
        release();
    }
    
    void init(const std::string& path, int lvl) {
        if (lvl < 0 || lvl > 10) {
            std::cout << "Level should be in the range [0-10]" << std::endl;
            exit(1);
        }
        
        release();
        
        _handle = dlopen(path.c_str(), RTLD_NOW);
        if (!_handle) {
            std::cerr << dlerror() << std::endl;
            exit(1);
        }
        _init = (f_init_t) _dlsym("init");
        _gen_move = (f_gen_move_t) _dlsym("gen_move");
        _destroy = (f_destroy_t) _dlsym("destroy");

        _ctx = _init(lvl);
    }

    void release() {
        if (_destroy)
            _destroy(_ctx);
        if (_handle)
            dlclose(_handle);
    }
    
    dir_t gen_move(Board& board) {
        return _gen_move(_ctx, board);
    }

private:
    void* _dlsym(const char* name) {
        dlerror();
        void* sym = dlsym(_handle, name);
        const char* err = dlerror();
        if (err) {
            std::cerr << err << std::endl;
            exit(1);
        }
        return sym;
    }
    f_init_t _init;
    f_gen_move_t _gen_move;
    f_destroy_t _destroy;
    void* _ctx;
    void* _handle;
};

#endif // CONTROLLER_HPP
