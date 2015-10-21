#include <r_util.h>
#include <r_types.h>
#include <r_asm.h>
#include <string.h>

static void str_op(char *c) {
	if ((c[0] <= 'Z') && (c[0] >= 'A'))
		c[0] += 0x20;
}

static int gbAsm(RAsm *a, RAsmOp *op, const char *buf) {
	int mn_len, i, len = 1;
	ut32 mn = 0;
	ut64 num;
	if (!a || !op || !buf)
		return 0;
	strncpy (op->buf_asm, buf, R_ASM_BUFSIZE);
	r_str_replace (op->buf_asm, "  ", " ", R_TRUE);
	r_str_replace (op->buf_asm, " ,", ",", R_TRUE);
	mn_len = r_str_do_until_token (str_op, op->buf_asm, ' ');
	if (mn_len < 2 || mn_len > 4)
		return 0;
	for (i = 0; i < mn_len; i++)
		mn = (mn << 8) | op->buf_asm[i];
	switch (mn) {
		case 0x6e6f70:			//nop
			op->buf[0] = 0x00;
			break;
		case 0x726c6361:		//rlca
			op->buf[0] = 0x07;
			break;
		case 0x72726361:		//rrca
			op->buf[0] = 0xf0;
			break;
		case 0x73746f70:		//stop
			op->buf[0] = 0x10;
			break;
		case 0x726c61:			//rla
			op->buf[0] = 0x17;
			break;
		case 0x727261:			//rra
			op->buf[0] = 0x1f;
			break;
		case 0x646161:			//daa
			op->buf[0] = 0x27;
			break;
		case 0x63706c:			//cpl
			op->buf[0] = 0x2f;
			break;
		case 0x736366:			//scf
			op->buf[0] = 0x37;
			break;
		case 0x636366:			//ccf
			op->buf[0] = 0x3f;
			break;
		case 0x68616c74:		//halt
			op->buf[0] = 0x76;
			break;
		case 0x726574:			//ret
			op->buf[0] = 0xc9;
			break;
		case 0x72657469:		//reti
			op->buf[0] = 0xd9;
			break;
		case 0x6469:			//di
			op->buf[0] = 0xf3;
			break;
		case 0x6569:			//ei
			op->buf[0] = 0xfb;
			break;
		case 0x727374:			//rst
			if (strlen (op->buf_asm) < 5)
				return op->size = 0;
			num = r_num_get (NULL, &op->buf_asm[4]);
			if ((num & 7) || ((num/8) > 7))
				return op->size = 0;
			op->buf[0] = (ut8)((num & 0xff) + 0xc7);
			break;
		case 0x70757368:		//push
			if (strlen (op->buf_asm) < 7)
				return op->size = 0;
			str_op (&op->buf_asm[5]);
			str_op (&op->buf_asm[6]);
			if (op->buf_asm[5] == 'b' && op->buf_asm[6] == 'c') {
				op->buf[0] = 0xc5;
			} else if (op->buf_asm[5] == 'd' && op->buf_asm[6] == 'e') {
				op->buf[0] = 0xd5;
			} else if (op->buf_asm[5] == 'h' && op->buf_asm[6] == 'l') {
				op->buf[0] = 0xe5;
			} else if (op->buf_asm[5] == 'a' && op->buf_asm[6] == 'f') {
				op->buf[0] = 0xf5;
			} else len = 0;
			break;
		case 0x706f70:			//pop	
			if (strlen (op->buf_asm) < 6)
				return op->size = 0;
			str_op (&op->buf_asm[4]);
			str_op (&op->buf_asm[5]);
			if (op->buf_asm[4] == 'b' && op->buf_asm[5] == 'c') {
				op->buf[0] = 0xc1;
			} else if (op->buf_asm[4] == 'd' && op->buf_asm[5] == 'e') {
				op->buf[0] = 0xd1;
			} else if (op->buf_asm[4] == 'h' && op->buf_asm[5] == 'l') {
				op->buf[0] = 0xe1;
			} else if (op->buf_asm[4] == 'a' && op->buf_asm[5] == 'f') {
				op->buf[0] = 0xf1;
			} else len = 0;
			break;
		default:
			len = 0;
			break;
	}
	return op->size = len;
}