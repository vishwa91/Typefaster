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
#ifndef HELPFILE_H
#define HELPFILE_H
#include <qstring.h>
#include <qapplication.h>

QString helpfile2 =

"<qt>"
"<h1>TypeFaster - Teacher's Help</h1>"
"<font size=5>"
"<p>"
"The version of the program you are using is designed for use in a school. "
"It must be installed "
"in one place only, for example in a shared network folder."
"In this version there are two types of users: ordinary users and users with "
"teacher privileges. You have teacher privileges."
"</p>"
"<u><h2>Usernames that teachers should use</h2></u>"
"<p>"
"When ordinary users (pupils) login, they must select a teacher's username. "
"The pupil will then "
"get the lesson files that that teacher specified. So there should be a teacher's name "
"for each different set of lesson files. An example of a teacher's login name is:<br/>"
"<b>Mr Smith</b><br/>"
"If Mr Smith teaches typing to two different grades then he should create a different "
"login for each grade, so for example, he could then create two users with teacher "
"privileges:<br/>"
"<b>Mr Smith's Grade 8s</b> and<br/>"
"<b>Mr Smith's Grade 9s</b><br/>"
"and give them different lesson files."
"</p>"
"<u><h2>Teacher privileges</h2></u>"
"<p>"
"The button <b>Edit class settings</b> allows you to:"
"<ul>"
"<li>Enable or disable keyboard layouts</li>"
"<li>Edit, create or delete lesson files for each keyboard layout</li>"
"<li>Edit the game settings (word list and speed)</li>"
"</ul>"
"</p>"
"<p>"
"The button <b>View class progress</b> allows you to view typing statistics for "
"each of your pupils. The statistics for a pupil in a keyboard layout are: "
"<ul>"
"<li><b>Attempted</b> - the percentage of keys in that layout that the "
"pupil has attempted.</li>"
"<li><b>Accuracy</b> - the overall accuracy of the pupil.</li>"
"<li><b>Speed</b> - the overall speed of the pupil.</li>"
"</ul>"
"If you are getting statistics about a pupil that is not in your class or has left "
"your class, you can press the <b>Exclude from statistics</b> button."
"</p>"
"<p>"
"The button <b>Delete students</b> can be used to completely remove pupils from "
"the system. Students that have been deleted will not be able to log in. "
"So students should only be deleted once they have left the school."
"</p>"
"</font>"
"</qt>";
//QT_TRANSLATE_NOOP("helpfile",
QString helpfile = 

#ifdef SPANISH
"<qt><h1>TypeFaster - Tutor de mecanografía - ayuda</h1>"
"<img  src=\"bike\"/>"
"<font size=5><p>Este programa fue diseñado para enseñar a las personas a mecanografiar. "
"La mecanografía es una habilidad que se puede aprender con práctica y perseverancia. "
"La mecanografía es un método de escritura que utiliza 8 dedos y un pulgar. "
#else
"<qt><h1>TypeFaster Typing Tutor Help</h1>"
"<img  src=\"bike\"/>"
"<font size=5><p>This program has been designed to teach people to touch-type. "
"Touch-typing is a skill that can be learnt with practice and "
"perseverance. "
"Touch-typing is a method of typing which uses 8 fingers and one "
"thumb."
#endif

#ifdef FEST
 #ifdef SPANISH
"Los mecanógrafos saben donde se encuentran las teclas en el teclado. "
"Esta versión del programa es específica para personas con problemas visuales o ciegas.</p>"
 #else
"Touch-typists know where the keys are located on the keyboard. "
"This version of the program is specifically for visually "
"impaired or blind people.</p>"
 #endif
#else
 #ifdef SPANISH
"Los mecanógrafos no tienen que ver el teclado para encontrar las teclas que quieren oprimir.</p>"
 #else
"Touchtypists do not have to look at the keyboard to find "
"the keys they want to type.</p>"
 #endif
#endif

#ifdef SPANISH
"<h2>Teclas base</h2>"
"<p>Cada dedo tiene su propio lugar para descansar.  Estas teclas donde descansan los dedos se llaman base "
"y se resaltan en azul en este programa.</p>"
#else
"<h2>Homekeys</h2>"
"<p>Each finger has its own resting place.  These resting keys are "
"called the homekeys.  The homekeys have a blue outline in this "
"program.</p>"
#endif

#ifdef FEST
 #ifdef SPANISH
"<p>Los ocho dedos largos, es decir, el dedo meñique, el anular, el dedo medio "
"y el dedo índice de cada mano, todos descansan el la tercer fila del lado de "
"el teclado más cercano a ti. "
 #else
"<p>The 8 long fingers, that is, the small finger, ring finger, middle finger "
"and index finger of each hand all rest on the, third row, from the side "
"of the keyboard closest to you. "
 #endif
#else
 #ifdef SPANISH 
"<p>Por ejemplo, en la disposición del teclado en español, "
"los cuatro dedos de la mando izquierda descansan en las teclas "
"'A','S','D','F'. El dedo meñique en la 'A', el anular en la 'S', el dedo medio en la "
"'D' y el dedo índice en la 'F'.  Los dedos de la mano derecha descansan en las teclas "
"'J','K','L','Ñ'. El dedo índice en la 'J', el dedo medio en la 'K', el anular en la 'L' "
"y el dedo meñique en la 'Ñ'. "
 #else
"<p>For example, in the standard U.S-English keyboard layout, "
"The four fingers of the left hand rest on the keys "
"'A','S','D','F' ie. small finger on 'A', ring finger on 'S', middle finger "
"on 'D' and index finger on 'F'.  The right hand fingers rest "
"on the keys 'J','K','L',';' ie index finger on 'J', middle finger on 'K', "
"ring finger on 'L', and small finger on ';'. "
 #endif
#endif

#ifdef SPANISH
" Las teclas base usualmente se localizan con facilidad en el teclado "
"porque en el lugar de los dedos índices las teclas base tienen un punto resaltado.</p>"
#else
" The homekeys can usually "
"be located easily on a keyboard because the left and right index "
"fingers' homekeys have a small bump on them.</p>"
#endif

#ifdef FEST
 #ifdef SPANISH
"<p> Una vez que hayas localizado el punto resaltado de la izquierda coloque su dedo índice izquierdo en esa tecla, "
"luego, coloque los otros 3 dedos de la mano izquierda en las teclas de la izquierda, todos en la misma fila. "
"Haga lo mismo con la mano derecha.  Descanse su dedo pulgar en la barra espaciadora, que es la tecla más ancha "
"en la fila más cercana a usted.</p>"
 #else
"<p> Once you have located the left bump, place your left index finger on that key "
"then place the other 3 fingers of the left hand on the keys to the left, all on the same row. Do "
"the same for the right hand.  Rest your right thumb on the space bar key, which is "
"the very wide key on the row closest to you.</p>"
 #endif
#endif

"<img src=\"homekeys\">"
#ifdef SPANISH
"<p>La imagen de arriba muestras como las teclas base "
"tienen recuadro azul.</p>"
#else
"<p>The above image shows how the homekeys on the U S English keyboard layout "
"are outlined in blue.</p>"
#endif

#ifndef FEST
 #ifdef SPANISH
"<p>También muestra que la tecla que debe ser oprimida es "
"'j' porque toda la tecla esta resaltada en rojo.</p>"
 #else
"<p>It also shows that the key that must be pressed "
"is 'f' because the whole key is highlighted in red.</p>"
 #endif
#endif

#ifdef SPANISH
"<p>En mecanografía cada tecla siempre se oprime con el mismo dedo, en otras palabras, "
"cada dedo sólo oprime ciertas teclas. "
"Asegúrese de que después de oprimir una tecla regrese su dedo a su posición de descanso. "
"La razón por lo que esto es importante es que en la mecanografía "
"usted aprende la posición relativa de las teclas. "
"Esto es, usted aprende donde está localizada una tecla en un movimiento desde cierta tecla base.</p>"
"<h2>Letras mayúsculas.</h2>"
"<p>La tecla cambiar a mayúsculas 'Shift' debe mantenerse oprimida mientras se oprime la letra que se desee en mayúscula. "
"La tecla 'shift' se oprime con el dedo meñique de la mano izquierda, "
"y la tecla 'shift' se oprime con el dedo meñique de la mano derecha.</p>"
#else
"<p>In touch-typing, each key is always pressed "
"by the same finger, in other words, each finger only presses certain keys. "
"Make sure that after pressing a key, you return "
"that finger to its resting place, that is, its homekey.  The reason that this is important "
"is because in touch-typing you learn the relative position of keys.  That is, "
"you learn where a key is based on a movement from a certain homekey.</p>"
"<h2>Capital Letters.</h2>"
"<p>Shift must be "
"held down while the key you wish to capitalize is pressed.  The left shift key is "
"pressed by the left hand's small finger, and the right shift key is pressed "
"by the right hand's small finger.</p>"
#endif

#ifdef FEST
 #ifdef SPANISH
"<p>Las teclas 'shift' son más anchas que las teclas normales.  La izquierda uno "
"se oprime moviendo su dedo un dedo hacia abajo (eso significa una línea más cerca de usted) "
"y ligeramente a la izquierda. "
"Para oprimir la tecla 'shift' derecha, mueva su dedo una línea hacia abajo y hacia la derecha.</p>"
 #else
"<p>The shift keys are wider than the standard keys.  The left shift key is pressed by "
"moving your left hand's small finger one row down (that means one row closer to you) "
"and slightly to the left. "
"To press the right shift, move your right hand's small finger one row down and then quite "
"far right.</p>"
 #endif
#endif

#ifdef SPANISH
"<h2>Qué dedos utilizar</h2>"
"<p>El programa resaltará en rojo la tecla que deba ser oprimida en el teclado. "
"Si la tecla que deba oprimirse no es una de las teclas base, también resaltará con cual "
"dedo deberá ser oprimida, resaltando la letra en la tecla base sobre la cual "
"el dedo descansa. "
#else
"<h2>Which fingers to use</h2>"
"<p>The program will highlight in red the key that needs to be pressed "
"on the keyboard.  If the key to be pressed is not one of the homekeys, "
"the program will also highlight which finger it must be pressed "
"by, by highlighting the letter on the homekey on which the "
"finger rests. "
#endif

#ifndef FEST
 #ifdef SPANISH
//"Enter se presiona por el, right small finger"
 #else
"Enter is pressed by the right small finger. "
 #endif
#endif

#ifdef SPANISH
"El pulgar derecho oprime la barra espaciadora aunque algunas personas prefieren hacerlo con el pulgar izquierdo "
"y otros usan ambos.</p>"
#else
"The right thumb presses the space bar although some people prefer "
"the left thumb and others use both.</p>"
#endif

#ifdef FEST
 #ifdef SPANISH
"<p>La voz le dirá cuál tecla necesita oprimir si no sabe donde está la tecla, "
"solo espere unos segundos y le informará donde está la tecla diciéndole la  "
"posición relativa de uno de sus dedos.</p>"
 #else
"<p>The voice will tell you which key you need to press.  If you do not know where a "
"key is, just wait a few seconds and it will tell you where the key is by telling you "
"the relative position from one of your fingers.</p>"
 #endif
#endif

#ifdef SPANISH
"<h2>La precisión</h2>"
"<p>La precisión es vital al aprender mecanografía. Trate de cometer los menos errores posibles. "
"Trate de mantener la precisión arriba de 95%. "
"Si comete un error, puede utiliza la tecla de retroceso (la tecla grande cerca del lado derecho superior "
"del teclado, antes de la parte numérica) para corregirlo. "
"La tecla de retroceso se oprime con el dedo meñique o anular de la mano derecha. " 
"La velocidad vendrá con la práctica y después de un tiempo ya sabrás donde están las teclas.</p>"
"<h2>Lecciones</h2>"
"<p>Le recomiendo que haga cada lección varias veces antes de pasar a la siguiente. "
"Las lecciones se complementan, por lo que es necesario tener buen dominio de todas " 
"las teclas de la lección antes de cambiar a la siguiente.</p>" 
#else
"<h2>Accuracy</h2>"
"<p>Accuracy is vital when learning to type.  Try to make as few "
"mistakes as possible.  Try to keep your accuracy rate above "
"95%.  If you make a mistake, you can use backspace (the big key near "
"the top right hand side of the keyboard, before the numeric keypad) "
"to correct it.  Backspace is " 
"pressed with the small or ring finger of the right hand. "
"Speed will come with practice and after a while you will "
"know where the keys are.</p>"
"<h2>Lessons</h2>"
"<p>I recommend that you do each lesson several times before "
"moving onto the next one.  The lessons build on to one another " 
"so it is important to be comfortable with all the keys in a lesson " 
"before moving onto the next lesson.</p>"
#endif

#ifdef FEST
 #ifdef SPANISH
"<p>Puede oprimir F1 para regresar a la lección previa y F2 para ir a la lección siguiente. "
"La tecla del lado superior izquierdo es escape, luego, a la derecha sigue F1, luego F2, etcétera.</p>"
 #else
"<p>You can press F1 to go to the previous lesson and F2 to go to the next lesson. "
"The top left key is escape, then the key to the right of that is F1, then to the right "
"of that F2, then F3, then F4.</p>"
 #endif
#endif
#ifndef FEST
 #ifdef SPANISH
"<h2>Teclado numérico</h2>"
"<p>Si desea aprender la sección numérica del teclado, cambie la plantilla de disposición del teclado a "
"Teclado numérico.  Asegúrese de la luz de bloqueo de números esté encendida (se puede encender oprimiendo y soltando "
"la tecla bloqueo de números que está en el lado superior izquierdo de la teclado numérico). "
"El dedo índice derecho descansa en el 4, el dedo medio descansa en el 5 y el dedo anular descansa en el 6. "
"El dedo meñique oprime Intro y el pulgar oprime el 0.</p>"
"<h2>Bloqueo de mayúsculas</h2>"
"<p>Antes de iniciar las lecciones asegúrese de que el bloqueo de mayúsculas esté desactivado.</p>"
"<p>Para generar ciertas letras es necesario oprimir dos teclas en secuencia.</p>"
"<img src=\"doubledot\" />"
"<p>La imagen de arriba muestra cómo indica esta programa que debe oprimir las dos teclas en secuencia "
"para generar una `i` con un acento en el teclado en español. "
"Primero haga la combinación que se ha resaltado en rojo sólido y luego haga la secuencia "
"que está resaltada en el patrón rojo.</p>"
"<p>Puede cerrar esta ventana ahora, pero puede regresar a ella en cualquier momento oprimiendo el botón de ayuda.</p>"
 #else
"<h2>Numeric Keypad</h2>"
"<p>If you want to learn the numeric keypad, change the keyboard layout from "
"US-English to Numeric Keypad.  Make sure the num lock light is on (it "
"can be switched on by pressing and releasing the num lock key which is at the "
"top left of the numeric keypad).  The right index finger rests on '4', the "
"middle finger rests on '5', the ring finger rests on '6'.  The small finger "
"presses enter and the thumb presses '0'.</p>"
"<h2>Caps Lock</h2>"
"<p>Before you start the lessons, make sure that caps lock is off.</p>"
"<h2>Pep Talk</h2>"
"<p>When learning to touchtype, it is important to persevere.  At "
"first, touchtyping will be slower than using two fingers "
"and looking at the keyboard.  However, after a while, touch"
"typing will be much faster and easier.  If you never learn the "
"positions of the keys, you will probably still be looking at the "
"keyboard for a long time (unless you really type a lot).</p>"
"<h2>For users of the US-English keyboard layout</h2>"
"<p>I have heard that the 'qwerty' (US-English) keyboard layout "
"was actually designed to prevent old typewriters from jamming (by "
"slowing the typist down).  Therefore if you have not yet learnt it, "
"you may want to consider learning the Dvorak keyboard layout instead. "  
"The Dvorak keyboard layout was carefully designed to maximise typing "
"speed.  If you want to do this, change the layout from " 
"US-English to Dvorak, and then "
"tell your operating system that you have a Dvorak keyboard (even if you don't). "
"In Windows 2000, this can be done by clicking on 'Start', 'Settings', 'Control "
"Panel', 'Keyboard'.  Click on the 'Input Locales' tab, click on 'Add' and in "
"the 'Keyboad layout' drop down list, click on 'United States-Dvorak'. "
"Then click on 'Set as Default' and then click on 'OK'.  You may "
"also want to add stickers to the keys on your keyboard whilst you are learning.</p>"
"<h2>For users of the French, Spanish, Portuguese, German, Norwegian or Danish keyboard layouts</h2>"
"<p>To generate certain letters, such as those with accents, it is necessary to "
"do two sequences of key presses.  For example if you want the letter 'a' but with "
"an accent on it, you first need to do the key combination that generates the accent "
"(although nothing will appear on screen), and then you need to press the 'a' key.</p>"
"<img src=\"doubledot\" />"
"<p>The above image shows how this program indicates that you must do two sequences "
"of key combinations to generate 'E' with two dots above it on the French keyboard "
"layout.  First do the combination that is highlighted in solid red and then do the "
"sequence that is highlighted in the patterned red</p>"
"<h2>Let the practice begin</h2><p>Place your fingers on the correct resting places "
"and press the key that is highlighted in red.  The keys you type will appear in the white "
"bar near the top of the screen.  The aim is to type what is written at the top "
"of the white bar.  You can close this window now, but you can always get back to "
"it by pressing the 'help' button</p>"
 #endif
#else
 #ifdef SPANISH
"<p>Coloque los dedos en su posición de descanso y siga las indicaciones de voz. "
"Si oprime la tecla correcta usted escuchará un sonido como de máquina de escribir "
"Si usted no está oprimiendo ninguna de las teclas correctamente podría deberse a que tiene activado el bloqueo de las mayúsculas. "
"El bloqueo de mayúsculas puede activarse o desactivarse oprimiendo la tecla a la izquierda del dedo meñique de la mano izquierda.</p>"
"<p>Para cerrar el programa mantenga oprimida la tecla Alt y luego oprima F4. "
"La tecla Alt está al lado izquierdo de la barra espaciadora.</p>"

//the following is a computer translation
"<p>Usted puede volver a esta ayuda apretando el escape (la llave izquierda primera). "
"Usted puede cerrar también esta ayuda apretando el escape, "
"pero cerrar automáticamente en breve.</p>"
/*
"<p>You can get back to this help by pressing escape (the top left key).  "
"You can also close this help by pressing escape, but it will close "
"automatically shortly.</p>"*/
 #else
"<p>Place your fingers on their resting places and follow the voice prompts. If you hit the "
"correct key, you should hear a type writer like sound.  If you are not getting "
"any of the keys correctly, it might be because caps lock is on. Caps lock can "
"be switched on and off by pressing the key to the left of the left hand's small finger.</p>"
"<h2>Shortcut Keys</h2>"
"<p>To close the program, press and hold the alt key and then press F4. The alt key is "
"the key to the left of the space bar.</p>"
"<p>Press F3 to increase the font size and F4 to decrease the font size.</p>"
"<p>You can get back to this help by pressing escape (the top left key).  "
"You can also close this help by pressing escape, but it will close "
"automatically shortly.</p>"
 #endif
#endif
"</font></qt>";

#endif


