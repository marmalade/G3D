using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Reflection;
using G3D.Converter.Converter;
using System.Xml.Linq;

namespace G3D.Converter.Batch {
    class Program {
        static void Main(string[] args) {
            using(ConverterMgr convert=new ConverterMgr()) {
                convert.output+=Output;

                if(args.Length<=0) {
                    Console.WriteLine("Please supply a file");
                    return;
                }
                for(int i=1;i<args.Length;i++) {
                    if(args[i]=="--force")
                        convert.force=true;
                    else
                        convert.export=args[i];
                }

                convert.Convert(args[0]);
            }
        }

        static void Output(ConverterMgr.MsgType type,string msg) {
            Console.WriteLine(type.ToString()+": "+msg);
            Debug.WriteLine(type.ToString()+": "+msg);
        }
    }
}

