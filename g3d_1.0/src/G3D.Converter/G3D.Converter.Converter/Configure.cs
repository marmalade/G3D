using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.IO;
using System.Xml;

namespace G3D.Converter.Converter {
    public abstract class ConfigureSection : ConvertItem<Configure,XElement> {
        public ConfigureSection(string name) : base(name) { }
    }
    public class Configure : ConvertItem<ConverterMgr,string> {
        public string filename;

        public Configure() : base("configure") { }
        protected override void Process(string _filename) {
            filename=_filename;

            XDocument doc=XDocument.Load(filename,LoadOptions.SetLineInfo);
            XElement root=doc.Root;

            if(!mgr.Process(name+"."+root.Name.LocalName,root)) {
                mgr.Error("not a valid configuration file: "+filename);
                return;
            }
        }
        public string GetLocation(XElement xNode) {
            if(((IXmlLineInfo)xNode).HasLineInfo())
                return string.Format("{0}({1:000},{2:000})",filename,((IXmlLineInfo)xNode).LineNumber,((IXmlLineInfo)xNode).LinePosition);
            else
                return string.Format("{0}",filename);
        }

        internal void ConvertSection(string _filename,string rootName) {
            filename=_filename;

            XDocument doc=XDocument.Load(filename,LoadOptions.SetLineInfo);
            XElement root=doc.Root;

            if(root.Name.LocalName != rootName) {
                mgr.Error("configuration file "+filename+" needs to have this root "+filename);
                return;
            }

            if(!mgr.Process(name+"."+root.Name.LocalName,root)) {
                mgr.Error("not a valid configuration file: "+filename);
                return;
            }
        }
        public string ReadAttrOrElem(XElement item,string name,string sep=",") {
            XAttribute attr=item.Attribute(name);
            if(attr!=null)
                return (string)attr;
            string result=string.Join(sep,item.Attributes(name).Select((x) => x.Value));
            if(result=="") result=null;
            return result;
        }
    }
}
