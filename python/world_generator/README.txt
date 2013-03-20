==============================================
= Webots© Python World Generator - WPWG
=  Loïc Matthey
=  April 2008
=
= Copyright© 2008 Loïc Matthey
=
= This program is free software: you can redistribute it and/or modify
= it under the terms of the GNU Lesser General Public License as published by
= the Free Software Foundation, either version 3 of the License, or
= (at your option) any later version.
=
= This program is distributed in the hope that it will be useful,
= but WITHOUT ANY WARRANTY; without even the implied warranty of
= MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
= GNU Lesser General Public License for more details.
=
= You should have received a copy of the GNU Lesser General Public License
= along with this program.  If not, see <http://www.gnu.org/licenses/>.
==============================================

Use this program to automatically generate Webots worlds, based on specific templates and a XML parameter world file.

* The XML is parsed in worldhandler.py, calling specific functions of the WorldBuilder
* The Webots world is created by worldbuilder.py, using Templates with free parameters.