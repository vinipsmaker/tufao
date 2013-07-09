void SomeObject::someMethod()
{
    qDebug() << this->x;
    emit someSignal();
}
