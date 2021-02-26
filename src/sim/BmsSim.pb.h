/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.4-dev */

#ifndef PB_BMSSIM_PB_H_INCLUDED
#define PB_BMSSIM_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _BmsIn {
    float current;
    bool charger_available;
} BmsIn;

typedef struct _BmsMetaData {
    int32_t messages_incoming;
} BmsMetaData;

typedef struct _BmsOut {
    bool status_led;
    bool charge_enable;
    bool shutdown_line;
} BmsOut;

typedef struct _CanMessage {
    uint32_t id;
    int64_t data;
} CanMessage;

typedef struct _Cell {
    int32_t index;
    float voltage;
    bool is_draining;
} Cell;

typedef struct _Therm {
    int32_t index;
    float voltage;
} Therm;

typedef struct _CellList {
    pb_size_t cells_count;
    Cell cells[90];
} CellList;

typedef struct _ThermList {
    pb_size_t therms_count;
    Therm therms[24];
} ThermList;

typedef struct _BmsData {
    pb_size_t which_data;
    union {
        BmsIn bmsIn;
        BmsOut bmsOut;
        CellList cell_list;
        ThermList therm_list;
        CanMessage can;
    } data;
} BmsData;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define BmsMetaData_init_default                 {0}
#define BmsData_init_default                     {0, {BmsIn_init_default}}
#define BmsIn_init_default                       {0, 0}
#define ThermList_init_default                   {0, {Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default, Therm_init_default}}
#define Therm_init_default                       {0, 0}
#define BmsOut_init_default                      {0, 0, 0}
#define CanMessage_init_default                  {0, 0}
#define CellList_init_default                    {0, {Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default, Cell_init_default}}
#define Cell_init_default                        {0, 0, 0}
#define BmsMetaData_init_zero                    {0}
#define BmsData_init_zero                        {0, {BmsIn_init_zero}}
#define BmsIn_init_zero                          {0, 0}
#define ThermList_init_zero                      {0, {Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero, Therm_init_zero}}
#define Therm_init_zero                          {0, 0}
#define BmsOut_init_zero                         {0, 0, 0}
#define CanMessage_init_zero                     {0, 0}
#define CellList_init_zero                       {0, {Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero, Cell_init_zero}}
#define Cell_init_zero                           {0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define BmsIn_current_tag                        1
#define BmsIn_charger_available_tag              2
#define BmsMetaData_messages_incoming_tag        1
#define BmsOut_status_led_tag                    1
#define BmsOut_charge_enable_tag                 2
#define BmsOut_shutdown_line_tag                 3
#define CanMessage_id_tag                        1
#define CanMessage_data_tag                      2
#define Cell_index_tag                           1
#define Cell_voltage_tag                         2
#define Cell_is_draining_tag                     3
#define Therm_index_tag                          1
#define Therm_voltage_tag                        2
#define CellList_cells_tag                       1
#define ThermList_therms_tag                     1
#define BmsData_bmsIn_tag                        1
#define BmsData_bmsOut_tag                       2
#define BmsData_cell_list_tag                    3
#define BmsData_therm_list_tag                   4
#define BmsData_can_tag                          5

/* Struct field encoding specification for nanopb */
#define BmsMetaData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    messages_incoming,   1)
#define BmsMetaData_CALLBACK NULL
#define BmsMetaData_DEFAULT NULL

#define BmsData_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,bmsIn,data.bmsIn),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,bmsOut,data.bmsOut),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,cell_list,data.cell_list),   3) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,therm_list,data.therm_list),   4) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,can,data.can),   5)
#define BmsData_CALLBACK NULL
#define BmsData_DEFAULT NULL
#define BmsData_data_bmsIn_MSGTYPE BmsIn
#define BmsData_data_bmsOut_MSGTYPE BmsOut
#define BmsData_data_cell_list_MSGTYPE CellList
#define BmsData_data_therm_list_MSGTYPE ThermList
#define BmsData_data_can_MSGTYPE CanMessage

#define BmsIn_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    current,           1) \
X(a, STATIC,   SINGULAR, BOOL,     charger_available,   2)
#define BmsIn_CALLBACK NULL
#define BmsIn_DEFAULT NULL

#define ThermList_FIELDLIST(X, a) \
X(a, STATIC,   REPEATED, MESSAGE,  therms,            1)
#define ThermList_CALLBACK NULL
#define ThermList_DEFAULT NULL
#define ThermList_therms_MSGTYPE Therm

#define Therm_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    index,             1) \
X(a, STATIC,   SINGULAR, FLOAT,    voltage,           2)
#define Therm_CALLBACK NULL
#define Therm_DEFAULT NULL

#define BmsOut_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     status_led,        1) \
X(a, STATIC,   SINGULAR, BOOL,     charge_enable,     2) \
X(a, STATIC,   SINGULAR, BOOL,     shutdown_line,     3)
#define BmsOut_CALLBACK NULL
#define BmsOut_DEFAULT NULL

#define CanMessage_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   id,                1) \
X(a, STATIC,   SINGULAR, INT64,    data,              2)
#define CanMessage_CALLBACK NULL
#define CanMessage_DEFAULT NULL

#define CellList_FIELDLIST(X, a) \
X(a, STATIC,   REPEATED, MESSAGE,  cells,             1)
#define CellList_CALLBACK NULL
#define CellList_DEFAULT NULL
#define CellList_cells_MSGTYPE Cell

#define Cell_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    index,             1) \
X(a, STATIC,   SINGULAR, FLOAT,    voltage,           2) \
X(a, STATIC,   SINGULAR, BOOL,     is_draining,       3)
#define Cell_CALLBACK NULL
#define Cell_DEFAULT NULL

extern const pb_msgdesc_t BmsMetaData_msg;
extern const pb_msgdesc_t BmsData_msg;
extern const pb_msgdesc_t BmsIn_msg;
extern const pb_msgdesc_t ThermList_msg;
extern const pb_msgdesc_t Therm_msg;
extern const pb_msgdesc_t BmsOut_msg;
extern const pb_msgdesc_t CanMessage_msg;
extern const pb_msgdesc_t CellList_msg;
extern const pb_msgdesc_t Cell_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define BmsMetaData_fields &BmsMetaData_msg
#define BmsData_fields &BmsData_msg
#define BmsIn_fields &BmsIn_msg
#define ThermList_fields &ThermList_msg
#define Therm_fields &Therm_msg
#define BmsOut_fields &BmsOut_msg
#define CanMessage_fields &CanMessage_msg
#define CellList_fields &CellList_msg
#define Cell_fields &Cell_msg

/* Maximum encoded size of messages (where known) */
#define BmsMetaData_size                         11
#define BmsData_size                             1803
#define BmsIn_size                               7
#define ThermList_size                           432
#define Therm_size                               16
#define BmsOut_size                              6
#define CanMessage_size                          17
#define CellList_size                            1800
#define Cell_size                                18

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
