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
#ifndef NUMANDTIME_H
#define NUMANDTIME_H

#include <qstring.h>

struct numandtime
	{
		QChar c;

		ulong num;
		ulong totaltime;
		ulong missed;

		int sheight;
		int sr,sg,sb;

		int aheight;
		int ar,ag,ab;

		numandtime(QChar);
		
		numandtime(QChar ch,ulong n,ulong t,ulong m);

		void reset();

//		bool operator<(const numandtime& r);
		
	};

#endif
