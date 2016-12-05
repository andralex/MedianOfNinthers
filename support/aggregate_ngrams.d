#!/home/andrei/bin/rdmd

void main(string[] args)
{
    import std.algorithm.searching, std.conv, std.stdio;
    string lastNgram;
    uint runningCount;
    foreach (line; stdin.byLine)
    {
        char[] ngram = line;
        line = line.find('\t');
        ngram = ngram[0 .. $ - line.length];
        line = line[1 .. $];
        auto year = parse!uint(line);
        line = line[1 .. $];
        auto count = parse!uint(line);
        if (ngram == lastNgram)
        {
            // Add to the running count
            runningCount += count;
        }
        else
        {
            if (lastNgram.length)
                writeln(lastNgram, '\t', runningCount, '\t', len(lastNgram));
            runningCount = count;
            //delete lastNgram;
            lastNgram = ngram.idup;
        }
    }
    writeln(lastNgram, '\t', runningCount, '\t', len(lastNgram));
}

size_t len(string w)
{
    import std.range, std.string;
    auto i = indexOf(w, '_');
    if (i != -1) w = w[0 .. i];
    return w.walkLength;
}
