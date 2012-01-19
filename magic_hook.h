/*
    ndless-elfloader Loads ELF files
    Copyright (C) 2011  Daniel Tang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//Adds a magic number for detecting hooks

#include <os.h>

#define ELF_MAGIC_HOOK 0x454c4600 //"ELF\0"
#define STRVAL(a) _STRVAL(a)
#define _STRVAL(a) #a

#undef HOOK_DEFINE
#define HOOK_DEFINE(hookname) \
	unsigned __##hookname##_end_instrs[4]; \
	extern unsigned __##hookname##_saved_sp; \
	__asm(STRINGIFY(__##hookname##_saved_sp) ": .long 0"); /* accessed with pc-relative instruction */ \
	void __##hookname##_body(void); \
	void __attribute__((naked)) hookname(void) { \
	    __asm volatile(" add pc, pc, #0");  /* skip magic number */ \
	    __asm volatile(" .long " STRVAL(ELF_MAGIC_HOOK) ); /* magic number for hook detection */ \
		__asm volatile(" stmfd sp!, {r0-r12,lr}"); /* used by HOOK_RESTORE_STATE() */ \
		/* save sp */ \
		__asm volatile( \
			" str r0, [sp, #-4] @ push r0 but don't change sp \n " \
			" adr r0," STRINGIFY(__##hookname##_saved_sp) "\n" \
			" str sp, [r0] \n" \
			" ldr r0, [sp, #-4] @ pop r0 but don't change sp \n" \
		); \
		 __##hookname##_body(); \
	} \
	void __##hookname##_body(void)
