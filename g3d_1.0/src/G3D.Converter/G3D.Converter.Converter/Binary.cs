using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

/*
 * Patterns of use:
 * 1) generate a single file:
 *      OpenSingleFile("path/to/file.name");
 *      OpenFile("4CHR");
 *      bw.Write(value);
 *      WriteString("string");
 *      CloseFile();
 *      CloseSingleFile();
 * 2) generate a wad file:
 *      OpenWad("path/to/file.g3d.wad");
 *      OpenFile("4CHR","test.item1");
 *      WriteReference("test.item2");
 *      CloseFile();
 *      OpenFile("4CHR","test.item2");
 *      bw.Write(value);
 *      CloseFile();
 *      CloseWad();
 */

namespace G3D.Converter.Converter
{
    public class Binary
    {
        class Header
        {
            long offset;
            uint size=0;
            public void Start(string type, Binary parent)
            {
                parent.bw.Write(type.ToCharArray().Select((c) => (byte)c).ToArray());
                offset = parent.fs.Position;
                Write(parent);
            }
            public void Finish(Binary parent)
            {
                size = (uint)(parent.fs.Position - offset) + 4;
                long pos = parent.fs.Position;

                parent.fs.Position = offset;
                Write(parent);
                parent.fs.Position = pos;
            }
            protected virtual void Write(Binary parent)
            {
                parent.bw.Write(size);
            }
        }
        class FileHeader : Header {
            public ushort version = 0;
            public ushort num = 0;
            public uint tables = 0;
            public uint strings = 0;
            public uint reloc = 0;

            public FileHeader(Binary parent) { Start("G3DW", parent);  }
            protected override void Write(Binary parent)
            {
                base.Write(parent);
                parent.bw.Write(version);
                parent.bw.Write(num);
                parent.bw.Write(tables);
                parent.bw.Write(strings);
                parent.bw.Write(reloc);
            }
        }
        enum TableType
        {
            GenerateID = 1,
            Patch = 2,
            HasStrings = 4,
        }
        class Item
        {
            public uint offset;
            public string name;
        }
        class Table : Header
        {
            public ushort ident = 0;
            public ushort num = 0;
            public TableType flags = TableType.GenerateID | TableType.HasStrings;
            public List<Item> items = new List<Item>();

            internal void Add(Binary parent, string name)
            {
                Item item = new Item();
                item.offset = (uint)parent.fs.Position;
                item.name = name;
                items.Add(item);
                num++;
            }
            protected override void Write(Binary parent)
            {
                base.Write(parent);
                parent.bw.Write(ident);
                parent.bw.Write(num);
                parent.bw.Write((uint)flags);
            }
        }

        public BinaryWriter bw = null;
        public FileStream fs = null;
        FileHeader header;

        Table currTable = null;
        List<Table> otherTables = new List<Table>();

        Header currHeader = null;
        Stack<Header> otherHeaders = new Stack<Header>();

        List<string> refStrings = new List<string>();
        List<uint> refOffsets = new List<uint>();

        public void OpenWad(string filename)
        {
            fs = new FileStream(filename, FileMode.Create);
            bw = new BinaryWriter(fs);

            header = new FileHeader(this);
        }
        public void OpenFile(string type,string name,bool newTable = false)
        {
            if (otherTables.Count + 1 > ushort.MaxValue)
                newTable = true;
            if (newTable || currTable == null)
            {
                if (currTable != null)
                    otherTables.Add(currTable);
                currTable = new Table();
            }

            currTable.Add(this,name);

            OpenFile(type);
        }
        public void OpenFile(string type)
        {
            if (currHeader != null)
                otherHeaders.Push(currHeader);
            currHeader = new Header();
            currHeader.Start(type, this);
        }
        public void CloseFile()
        {
            currHeader.Finish(this);
            if (otherHeaders.Count > 0)
                currHeader = otherHeaders.Pop();
            else
                currHeader = null;
        }
        public void CloseWad()
        {
            otherTables.Add(currTable);

            header.num = (ushort)otherTables.Count;
            header.tables = (uint)fs.Position;

            for (int i = 0; i < otherTables.Count; i++)
            {
                otherTables[i].Start("TABL", this);
                for (int j = 0; j < otherTables[i].items.Count; j++)
                {
                    bw.Write(otherTables[i].items[j].offset);
                }
                for (int j = 0; j < otherTables[i].items.Count; j++)
                {
                    WriteString(otherTables[i].items[j].name);
                }   
                otherTables[i].Finish(this);
            }

            Header strTable = new Header();
            header.strings = (uint)fs.Position;
            strTable.Start("IMPT",this);
            bw.Write((uint)refStrings.Count);
            foreach(string str in refStrings)
                WriteString(str);
            strTable.Finish(this);

            Header refTable = new Header();
            header.reloc = (uint)fs.Position;
            refTable.Start("RELC",this);
            bw.Write((uint)refOffsets.Count);
            foreach(uint offset in refOffsets)
                bw.Write(offset);
            refTable.Finish(this);

            header.Finish(this);
            bw.Close();
            fs.Close();
        }

        public void OpenSingleFile(string filename) {
            fs = new FileStream(filename,FileMode.Create);
            bw = new BinaryWriter(fs);
        }
        public void CloseSingleFile() {
            bw.Close();
            fs.Close();
        }

        public void WriteString(string str)
        {
            Encoding enc = Encoding.UTF8;
            byte[] data = enc.GetBytes(str);
            bw.Write(data);
            bw.Write((byte)0);
        }
        public void WriteBool(bool value) {
            bw.Write(value?(byte)1:(byte)0);
        }
        public void WriteReference(string reference)
        {
            int num = refStrings.IndexOf(reference);
            if (num == -1)
            {
                refStrings.Add(reference);
                num = refStrings.Count - 1;
            }
            refOffsets.Add((uint)fs.Position);
            bw.Write(num);
        }

        public void Test()
        {
            OpenWad("test.wad");
            OpenFile("MAT ", "test.material1");
            WriteReference("test.texture1");
            CloseFile();
            OpenFile("TEXT", "test.texture1");
            CloseFile();
            CloseWad();
        }
    }
}
