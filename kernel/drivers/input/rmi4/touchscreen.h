#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

typedef enum {
	ID_MAIN		= 0,
	ID_SUB		= 1,
	ID_INVALID	= 2,
} touch_id_type;

typedef enum {
	MODE_INVALID		= 0,
	MODE_NORMAL		= 1,
	MODE_HANDWRITE		= 2,
	MODE_MAX		= 3,
} touch_mode_type;

typedef enum {
	OREITATION_INVALID	= 0,
	OREITATION_0		= 1,
	OREITATION_90		= 2,
	OREITATION_180		= 3,
	OREITATION_270		= 4,
} touch_oreitation_type;

typedef struct touchscreen_funcs {
	touch_id_type touch_id;
	int touch_type;
	int (*active)(void);
	int (*firmware_need_update)(void);
	int (*firmware_do_update)(void);
	int (*need_calibrate)(void);
	int (*calibrate)(void);
	int (*get_firmware_version)(char *);
	int (*reset_touchscreen)(void);
	touch_mode_type (*get_mode)(void);
	int (*set_mode)(touch_mode_type);
	touch_oreitation_type (*get_oreitation)(void);
	int (*set_oreitation)(touch_oreitation_type);
	int (*read_regs)(char *);
	int (*write_regs)(const char *);
	int (*debug)(int);
} touchscreen_ops_tpye;

#endif /* _TOUCHSCREEN_H */

/**********************************************************************
* �������ƣ�touchscreen_set_ops

* �������������ô������ڵ��������

* ���������touchscreen_ops_tpye

* ���������NONE

* ����ֵ      ��0---�ɹ���-1---ʧ��

* ����˵����

* �޸�����         �޸���	              �޸�����
* --------------------------------------------------------------------
* 2011/11/19	   �봺��                  �� ��
**********************************************************************/
extern int touchscreen_set_ops(touchscreen_ops_tpye *ops);
