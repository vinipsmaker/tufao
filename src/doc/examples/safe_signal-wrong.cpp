void SomeObject::someMethod()
{
    emit someSignal();
    qDebug() << this->x;
}
