/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "numandtime.h"

numandtime::numandtime(QChar ch)
		{
			c = ch;
			num = 0;
			totaltime = 0;
			missed = 0;
			
		}

numandtime::numandtime(QChar ch,ulong n,ulong t,ulong m)
		{
			c = ch;
			num = n;
			totaltime = t;
			missed = m;
		}

void numandtime::reset()
{
	num = 0;
	totaltime = 0;
	missed = 0;
}

/*
bool numandtime::operator<(const numandtime& r) 
{
	return c < r.c;   
}
*/
