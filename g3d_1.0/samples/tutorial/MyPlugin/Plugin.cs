using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using G3D.Converter.Converter;
using System.Xml.Linq;

namespace MyPlugin
{
    public class ControlModule : GOBModule
    {
        public float speed = 1.0f;
        public override void Load(XElement item)
        {
            LoadBase(item, "control", true);

            string xSpeed = null;
            if (item != null)
            {
                xSpeed = (string)item.Attribute("speed");
            }
            if (xSpeed != null)
            {
                speed = float.Parse(xSpeed);
            }
        }
        public override void Save(Binary binary)
        {
            binary.bw.Write(speed);
        }
    }

    public class Plugin : IConvertModule
    {
        public void StartExport() {}
        public void Process(object item) {}
        public void Setup(ConverterMgr mgr, IConvertItem parent)
        {
            mgr.SetModuleFactory("control", () => new ControlModule());
        }

        public string name { get { return "myplugin"; } }
    }
}
