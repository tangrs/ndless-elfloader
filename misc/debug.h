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
//#define DEBUG 1
#define assert(x)   do { \
                        if (!(x)) { \
                            console_printf("Assert failure at line %d in %s\nExpression was %s", __LINE__,__FILE__, #x); \
                            while (1); \
                        } \
                    } while (0)
