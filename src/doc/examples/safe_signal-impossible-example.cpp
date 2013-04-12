void Foobar::slot()
{
    QByteArray buf = socket->readAll();
    while (buf.size()) {
        // ...

        if (/* ... */)
            emit unsafeSignal();

        // ...
    }
}
