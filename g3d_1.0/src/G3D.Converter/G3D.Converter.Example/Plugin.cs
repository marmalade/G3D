using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using G3D.Converter.Converter;

namespace G3D.Converter.Example {
    public class RotatorModule : GOBModule {
        public float speed=1.0f;
        public override void Load(XElement item) {
            LoadBase(item,"ai",true);

            string xSpeed=null;
            if(item!=null) {
                xSpeed=(string)item.Attribute("speed");
            }
            if(xSpeed!=null) {
                speed=float.Parse(xSpeed);
            }
        }
        public override void Save(Binary binary) {
            binary.bw.Write(speed); 
        }
    }
    public class ExampleModule : IConvertModule {
        public string name { get { return "example"; } }
        protected ConverterMgr mgr;

        public void Setup(ConverterMgr _mgr,IConvertItem _parent) {
            mgr=_mgr;
            mgr.SetModuleFactory("rotator",() => new RotatorModule());
        }
        public void StartExport() {
        }
        public void Process(object item) {
        }
    }
}
