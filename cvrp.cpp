#include "php_CVRP.h"
#include "routermanager.h"

#include <iostream>

zend_object_handlers routermanager_object_handlers;

struct routermanager_object {
	zend_object std;
	RouterManager *routermanager;
};

void routermanager_free_storage(void *object TSRMLS_DC)
{
	routermanager_object *obj = (routermanager_object *)object;
	delete obj->routermanager;

	zend_hash_destroy(obj->std.properties);
	FREE_HASHTABLE(obj->std.properties);

	efree(obj);
}

zend_object_value routermanager_create_handler(zend_class_entry *type TSRMLS_DC)
{
	zend_object_value retval;

	routermanager_object *obj = (routermanager_object *)emalloc(sizeof(routermanager_object));
	memset(obj, 0, sizeof(routermanager_object));
	obj->std.ce = type;

	ALLOC_HASHTABLE(obj->std.properties);
	zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	object_properties_init(&obj->std, type);
	retval.handle = zend_objects_store_put(obj, NULL,
										   routermanager_free_storage, NULL TSRMLS_CC);
	retval.handlers = &routermanager_object_handlers;

	return retval;
}

zend_class_entry *routermanager_ce;

PHP_METHOD(RouterManager, __construct)
{
	std::cout << "try to construct" << std::endl;
	std::cout.flush();
	zval *object = getThis();
	char *folder;
	int l;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &folder, &l) == FAILURE) {
		std::cout << "couldnt parse" <<std::endl;
		RETURN_NULL();
	}
	std::cout << "parse  " << folder << std::endl;
	RouterManager *routerM = new RouterManager(folder);
	routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(object TSRMLS_CC);
	obj->routermanager = routerM;
}

PHP_METHOD(RouterManager, calculate)
{
	std::cout << "try to calculate";
	routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
				getThis() TSRMLS_CC);
	RouterManager *routermanager = obj->routermanager;
	if (routermanager != NULL) {
		routermanager->calculate();
	}
}

PHP_METHOD(RouterManager, isFinished)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL)
    {
        RETURN_NULL();
    }
    bool finished = routermanager->isFinished();
    RETURN_BOOL(finished);
}

PHP_METHOD(RouterManager, getUsedTrucks)
{
	routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
				getThis() TSRMLS_CC);
	RouterManager *routermanager = obj->routermanager;
	if (routermanager == NULL) {
		RETURN_NULL();
	}
	std::vector<unsigned int> trucksLs = routermanager->getUsedTrucks();
	//std::cout << "passing points num " << passingPntsLs.size() << "\n path length is " << path.size() <<"\n";
	HashTable *resTable;
	ALLOC_HASHTABLE(resTable);
	zend_hash_init(resTable, 1000000, NULL, NULL, 0);
	for (int i = 0; i < trucksLs.size(); i ++)
	{
		int id = trucksLs[i];
		zval *zId;
		MAKE_STD_ZVAL(zId);
		ZVAL_LONG(zId, id);
		if (zend_hash_next_index_insert(resTable, &zId, sizeof(zval*), NULL) == SUCCESS) {
			Z_ADDREF_P(zId);
		}
	}

	zval* res;
	MAKE_STD_ZVAL(res);
	Z_TYPE_P(res) = IS_ARRAY;
	Z_ARRVAL_P(res) = resTable;
	//zval_ptr_dtor(&res);
	//Z_ADDREF_PP(res);
	bool copy = false;
	bool dtor = false;
	RETURN_ZVAL(res, NULL, NULL);
}

PHP_METHOD(RouterManager, getRouteByTruck)
{
	routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
				getThis() TSRMLS_CC);
	RouterManager *routermanager = obj->routermanager;
	if (routermanager == NULL) {
		RETURN_NULL();
	}
	int routeId = -1;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &routeId) == FAILURE) {
	  RETURN_NULL();
	}

	std::vector<unsigned int> customersLs = routermanager->getRouteByTruck(routeId);
	//std::cout << "passing points num " << passingPntsLs.size() << "\n path length is " << path.size() <<"\n";
	HashTable *resTable;
	ALLOC_HASHTABLE(resTable);
	zend_hash_init(resTable, 1000000, NULL, NULL, 0);
	for (int i = 0; i < customersLs.size(); i ++)
	{
		int id = customersLs[i];
		zval *zId;
		MAKE_STD_ZVAL(zId);
		ZVAL_LONG(zId, id);
		if (zend_hash_next_index_insert(resTable, &zId, sizeof(zval*), NULL) == SUCCESS) {
			Z_ADDREF_P(zId);
		}
	}

	zval* res;
	MAKE_STD_ZVAL(res);
	Z_TYPE_P(res) = IS_ARRAY;
	Z_ARRVAL_P(res) = resTable;
	//zval_ptr_dtor(&res);
	//Z_ADDREF_PP(res);
	bool copy = false;
	bool dtor = false;
	RETURN_ZVAL(res, NULL, NULL);
}

PHP_METHOD(RouterManager, getRouteNoTruck)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
                getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL) {
        RETURN_NULL();
    }
    int routeId = -1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &routeId) == FAILURE) {
      RETURN_NULL();
    }

    std::vector<unsigned int> customersLs = routermanager->getRouteNoTruck(routeId);
    //std::cout << "passing points num " << passingPntsLs.size() << "\n path length is " << path.size() <<"\n";
    HashTable *resTable;
    ALLOC_HASHTABLE(resTable);
    zend_hash_init(resTable, 1000000, NULL, NULL, 0);
    for (int i = 0; i < customersLs.size(); i ++)
    {
        int id = customersLs[i];
        zval *zId;
        MAKE_STD_ZVAL(zId);
        ZVAL_LONG(zId, id);
        if (zend_hash_next_index_insert(resTable, &zId, sizeof(zval*), NULL) == SUCCESS) {
            Z_ADDREF_P(zId);
        }
    }

    zval* res;
    MAKE_STD_ZVAL(res);
    Z_TYPE_P(res) = IS_ARRAY;
    Z_ARRVAL_P(res) = resTable;
    //zval_ptr_dtor(&res);
    //Z_ADDREF_PP(res);
    bool copy = false;
    bool dtor = false;
    RETURN_ZVAL(res, NULL, NULL);
}

PHP_METHOD(RouterManager, routeNoTruckNum)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL) {
        RETURN_NULL();
    }
    int num = routermanager->routeNoTruckNum();
    RETURN_LONG(num);
}

PHP_METHOD(RouterManager, routeLengthByTruck)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
                getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL) {
        RETURN_NULL();
    }
    int routeId = -1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &routeId) == FAILURE) {
      RETURN_NULL();
    }

    double length = routermanager->routeLengthByTruck(routeId);
    RETURN_DOUBLE(length);
}

PHP_METHOD(RouterManager, routeBeginTimeByTruck)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
                getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL) {
        RETURN_NULL();
    }
    int routeId = -1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &routeId) == FAILURE) {
      RETURN_NULL();
    }

    double beginTime = routermanager->routeBeginTimeByTruck(routeId);
    RETURN_DOUBLE(beginTime);
}

PHP_METHOD(RouterManager, routeEndTimeByTruck)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(
                getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL) {
        RETURN_NULL();
    }
    int routeId = -1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &routeId) == FAILURE) {
      RETURN_NULL();
    }

    double endTime = routermanager->routeEndTimeByTruck(routeId);
    RETURN_DOUBLE(endTime);
}

PHP_METHOD(RouterManager, getProgress)
{
    routermanager_object *obj = (routermanager_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RouterManager *routermanager = obj->routermanager;
    if (routermanager == NULL) {
        RETURN_NULL();
    }
    int pr = routermanager->getProgress();
    RETURN_LONG(pr);
}

zend_function_entry routermanager_methods[] = {
	PHP_ME(RouterManager, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(RouterManager, calculate, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(RouterManager, getUsedTrucks, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(RouterManager, getRouteByTruck, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, routeLengthByTruck, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, routeBeginTimeByTruck, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, routeEndTimeByTruck, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, isFinished, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, routeNoTruckNum, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, getRouteNoTruck, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(RouterManager, getProgress, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(CVRP)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "RouterManager", routermanager_methods);
	routermanager_ce = zend_register_internal_class(&ce TSRMLS_CC);
	routermanager_ce->create_object = routermanager_create_handler;
	memcpy(&routermanager_object_handlers,
		   zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	routermanager_object_handlers.clone_obj = NULL;
	return SUCCESS;
}

zend_module_entry CVRP_module_entry = {
	#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
	#endif
	PHP_CVRP_EXTNAME,
	NULL,                  /* Functions */
	PHP_MINIT(CVRP),
	NULL,                  /* MSHUTDOWN */
	NULL,                  /* RINIT */
	NULL,                  /* RSHUTDOWN */
	NULL,                  /* MINFO */
	#if ZEND_MODULE_API_NO >= 20010901
	PHP_CVRP_EXTVER,
	#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CVRP
extern "C" {
ZEND_GET_MODULE(CVRP)
}
#endif
