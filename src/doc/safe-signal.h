/*
  Copyright (c) 2013 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

/*!
  \page safe-signal Safe signals

  We say that a signal is safe when it's safe to delete the object in the slot
  connected to this signal. This property implies that no member-function will
  access the attributes of the object after emit the signal (and won't schedule
  such accesses).

  Consider the following code:

  \include safe_signal-wrong.cpp

  In the previous code, _someSignal_ is an unsafe signal, because there is
  accesses to the internal state of the object after it emits the signal. We can
  refactor the previous code to make _someSignal_ safe by reordering. See the
  final version, where _someSignal_ is safe:

  \include safe_signal-hello_world.cpp

  Why do you wanna a safe signal? They are **safer** and you need to worry less
  about invalid access. Consider you have an object _O_ -- allocated on the heap
  -- of type _SomeObject_ and a method _M_ connected to the signal _someSignal_,
  then the following code is valid:

  \include safe_signal-using.cpp

  If you have an unsafe signal, your only choice is to use QObject::deleteLater.

  \section safe-signal-impossible Why some signals are unsafe

  Sometimes, it's too complicated to create a safe signal. Consider you receive
  a message via the network and some method parse this message. Consider the
  method should emit signals each time a new message is ready, something like
  the code below:

  \include safe_signal-impossible-example.cpp

  The above code uses an unsafe signal. Sure you could convert the code to use
  safe signals, but the overhead of performance and responsiveness created
  wouldn't be worth.

  Another example of an unsafe signal:

  \include safe_signal-impossible-example2.cpp

  \section safe-signal-conclusion Conclusion

  __Every signal in Tufão is safe, unless explicitly stated the opposite.__

  \warning
  It's unsafe to _delete_ an object in the body of a slot connected to an unsafe
  signal emitted by the same object.

  Some alternative ways of _deleting_ an object in code triggered by an unsafe
  signal are:
    - Call QObject::deleteLater
    - Use a Qt's _Qt::QueuedConnection_ connection to make the connection
      between the unsafe signal and your slot
    - Queuing/delegating the responsability to somebody else
    - Use some well designed architeture on top of QSharedPointer or similar
 */
