void main(string[] args)
{
    import std.format, std.stdio;
    double x;
    foreach (line; stdin.byLine)
    {
        line.formattedRead("%f", &x) == 1 || assert(0);
        stdout.rawWrite((&x)[0 .. 1]);
    }
}
