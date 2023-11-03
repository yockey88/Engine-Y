using System;
using System.Runtime.CompilerServices;

public class EditorCalls {
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void EditorLog(String line);
}