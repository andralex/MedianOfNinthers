void main(string[] args)
{
    import std.exception, std.getopt, std.random, std.stdio;
    double minValue = -300_000, maxValue = 300_000;
    string kind;
    size_t n = 10_000_000;
    uint seed = 1;

    args.getopt(
        "n", &n,
        "kind", &kind,
        "seed", &seed,
        "min", &minValue,
        "max", &maxValue);

    if (kind == "random")
    {
        auto rng = Random(seed ? seed : unpredictableSeed);
        foreach (i; 0 .. n)
        {
            //writeln(uniform(minValue, maxValue, rng), ',');
            double x = uniform(minValue, maxValue, rng);
            stdout.rawWrite((&x)[0 .. 1]);
            //import core.stdc.stdio;
            //fwrite(&x, x.sizeof, 1, stdout) == 1 || assert(0);
        }
    }
    else if (kind == "random01")
    {
        enforce(n % 2 == 0);
        auto rng = Random(seed ? seed : unpredictableSeed);
        auto a = new double[n];
        a[0 .. $ / 2] = 0;
        a[$ / 2] = 0.5;
        a[$ / 2 + 1 .. $] = 1;
        randomShuffle(a, rng);
        stdout.rawWrite(a);
    }
    else if (kind == "sorted")
    {
        foreach (double i; 0 .. n)
        {
            stdout.rawWrite((&i)[0 .. 1]);
        }
    }
    else if (kind == "rotated")
    {
        foreach (i; 1 .. n + 1)
        {
            double x = i;
            stdout.rawWrite((&x)[0 .. 1]);
        }
        double last = 0;
        stdout.rawWrite((&last)[0 .. 1]);
    }
    else if (kind == "organpipe")
    {
        foreach (double i; 0 .. n / 2)
        {
            stdout.rawWrite((&i)[0 .. 1]);
        }
        foreach_reverse (double i; 0 .. n / 2)
        {
            stdout.rawWrite((&i)[0 .. 1]);
        }
    }
    else if (kind == "m3killer")
    {
        enforce((n & 3) == 0);
        auto k = n / 2;
        foreach (i; 1 .. k + 1)
        {
            if (i & 1)
            {
                double x = i;
                stdout.rawWrite((&x)[0 .. 1]);
            }
            else
            {
                double x = k + i - 1;
                stdout.rawWrite((&x)[0 .. 1]);
            }
        }
        foreach (i; 1 .. k + 1)
        {
            double x = 2 * i;
            stdout.rawWrite((&x)[0 .. 1]);
        }
    }
    else
    {
        enforce(0, "Must specify --kind");
    }
}
