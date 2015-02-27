#ifndef PHP_CVRP_H
#define PHP_CVRP_H

#define PHP_CVRP_EXTNAME "CVRP"
#define PHP_CVRP_EXTVER "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
}

extern zend_module_entry CVRP_module_entry;
#define phpext_CVRP_ptr &CVRP_module_entry;

#endif // PHP_CVRP_H
