using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Globalization;

namespace DataStructs2
{
    public partial class Form1 : Form
    {
        private List<PC> PCs = new List<PC>();
        private List<GraphNode> AllGraphNodes = new List<GraphNode>();
        private List<Video> AllVideos = new List<Video>();
        private HashSet<Video> AllVideoH = new HashSet<Video>();
        private List<TreeNode> AllTreeNs = new List<TreeNode>();
        private int [,] GraphM = new int[8,8];
        private List<Video> TempVideosB = new List<Video>();

        public Form1()
        {
            InitializeComponent();
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    if(i!=j)
                        GraphM[i, j] = 65535;
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            PrepareF(ref PCs,ref AllVideos);
            TreeViewAddNodes(AllVideos);
            HashVideoLists(AllVideos, AllVideoH);
        }
        //窗口第一次显示时
        private void Form1_Shown(object sender, EventArgs e)
        {
            this.radioButton1.Checked = true;
            this.comboBoxPCs.SelectedIndex = 0;
        }

        ///准备文件工作
        void PrepareF(ref List<PC> pcs, ref List<Video> videos)
        {
            string path = this.folderBrowserDialog1.SelectedPath;//请求用户选择路径         
            if (!GetGraphFromF(path))//调用构图函数进行构图，如果路径为空无法构图则继续选择
            {
                this.folderBrowserDialog1.ShowDialog();
                PrepareF(ref pcs, ref videos);
            }
            else
            {
                foreach (var item in pcs)//计算机遍历文件
                {
                    item.GetVideos(path);
                    videos.AddRange(item.Videos);
                }
                this.comboBoxPCs.Items.AddRange(pcs.Select(p => p.Name).ToArray());
            }
        }

        /// 从拓扑数据库构建遍历图
        bool GetGraphFromF(string path)
        {
            try
            {
                if (PCs.Count > 0)
                    return true;
                StreamReader sr = new StreamReader(path + "\\网络拓扑.dat");
                string line;
                while ((line = sr.ReadLine()) != null)
                {
                    if (line.Contains("node:"))
                    {
                        foreach (string s in sr.ReadLine().Split(','))
                            PCs.Add(new PC { Name = s.Trim(new char[] { '(', ')', ';', ',', ' ' }) });
                    }
                    if (line.StartsWith("("))
                    {
                        string[] pcs = line.Split(',');
                        GraphM[GetPCIntByN(pcs[0]), GetPCIntByN(pcs[1])] = int.Parse(pcs[2].Trim(new char[] { '(', ')', ';', ',', ' ' }));
                        GraphM[GetPCIntByN(pcs[1]), GetPCIntByN(pcs[0])] = int.Parse(pcs[2].Trim(new char[] { '(', ')', ';', ',', ' ' }));
                    }
                }
                sr.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }

        //选好节点时，列出所有当下视频
        private void treeViewFiles_AfterSelect(object sender, TreeViewEventArgs e)
        {
            this.listViewSubFile.Items.Clear();
            TempVideosB.Clear();
            WalkFileTree(this.treeViewFiles.SelectedNode,AllVideos, new FunctionNode(addNodeToListView));
            GetListViewSort();
        }

        private void HashVideoLists(List<Video> videos,HashSet<Video> videoHash)
        {
            videoHash = new HashSet<Video>();
            videoHash.UnionWith(videos);
        }
        void GetListViewSort()
        {
            foreach (var item in this.groupBox1.Controls)
                radioButton1_CheckedChanged(item, null);
        }
        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton rb = (RadioButton)sender;
            if (rb.Checked == false)
                return;
            string[] arr = ListItemToArray(rb.TabIndex);
            int[] result = new int[arr.Length];
            for (int i = 0; i < arr.Length; i++)
                result[i] = i;
            switch(rb.TabIndex)
            {
                case 0:
                    QucikSort(result, arr, 0, result.Length - 1);
                    break;
                case 1:
                    BubbleSort(result, arr);
                    break;
                case 2:
                    SelectSort(result, arr);
                    break;
            }
            ArryToListView(result);
        }

        string[] ListItemToArray(int type)
        {
            //_tempVideosBeforeSort = new List<Video>();
            List<string> result = new List<string>();
            foreach (Video item in TempVideosB)
            {
                switch(type)
                {
                    case 0:
                        result.Add(item.File.Name);
                        break;
                    case 1:
                        result.Add(item.File.Extension);
                        break;
                    case 2:
                        result.Add(item.File.Length.ToString());
                        break;

                }
                
                //_tempVideosBeforeSort.Add(item.Tag as Video);
            }
            return result.ToArray();

        }

        void ArryToListView(int[] result)
        {
            ListView.ListViewItemCollection lis = this.listViewSubFile.Items;
            this.listViewSubFile.Items.Clear();
            foreach (int i in result)
            {
                Video v = TempVideosB[i];
                ListViewItem li = new ListViewItem();
                li.Text = v.File.Name;
                li.SubItems.Add(v.File.Extension);
                li.SubItems.Add(v.File.Length.ToString());
                li.SubItems.Add(v.Location.Name);
                this.listViewSubFile.Items.Add(li);
            }
        }
        /// <summary>
        /// 快速排序
        /// </summary>
        /// <param name="a">序列数组</param>
        /// <param name="r">名称数组</param>
        /// <param name="left">左标</param>
        /// <param name="right">右标</param>
        private void QucikSort(int[] a,string[] r, int left, int right)
        {
            if (left < right)
            {
                int i = Partition(a,r, left, right);
                QucikSort(a,r, left, i - 1);
                QucikSort(a,r, i + 1, right);
            }
        }

        private int Partition(int[] a, string[] r, int left, int right)
        {

            string tmp = r[left];
            int t = a[left];
            while (left < right)
            {
                while (left < right && String.Compare(r[right] ,tmp)>=0)
                    right--;

                // 换位后不能将left+1,防止跳位
                if (left < right)
                {
                    r[left] = r[right];
                    a[left] = a[right];
                }

                while (left < right && String.Compare(r[left], tmp) <= 0)
                    left++;

                if (left < right)
                {
                    r[right] = r[left];
                    a[right] = a[left];
                    // 有left < right，可将right向前推一位
                    right--;
                }
            }

            r[left] = tmp;
            a[left] = t;
            return left;
        }

        void BubbleSort(int[] a,string[] r)
        {
            for (int j = 1; j < r.Length; j++)
            {
                for (int i = 0; i < r.Length - 1; i++)
                {
                    // 如果 myArray[i] > myArray[i+1] ，则 myArray[i] 上浮一位
                    if (String.Compare(r[i] , r[i + 1])>0)
                    {
                        string temp = r[i];
                        r[i] = r[i + 1];
                        r[i + 1] = temp;
                        int tem = a[i];
                        a[i] = a[i + 1];
                        a[i + 1] = tem;
                    }
                }
            }
        }
        public void SelectSort(int[] a,string[] r)
        {
            int min;
            for (int i = 0; i < r.Length - 1; i++)
            {
                min = i;
                for (int j = i + 1; j < r.Length; j++)
                {
                    if (String.Compare(r[j],  r[min])<0)
                        min = j;
                }
                string temp = r[min];
                r[min] = r[i];
                r[i] = temp;
                int tem = a[min];
                a[min] = a[i];
                a[i] = tem;
            }
        }

        //更新按钮
        private void btnReflash_Click(object sender, EventArgs e)
        {
            //List<PC> pcs = new List<PC>();
            List<Video> videos = new List<Video>();
            PrepareF(ref PCs, ref videos);
            TreeViewAddNodes(videos);
            treeViewFiles_AfterSelect(null, null);
        }

        //搜索
        private void buttonSearch_Click(object sender, EventArgs e)
        {
            string key = this.textBoxSearch.Text;
            //搜索
            TempVideosB = AllVideos.FindAll(v => v.File.Name.Contains(key.Trim()));
            this.listViewSubFile.Items.Clear();
            List<ListViewItem> lis = new List<ListViewItem>();
            foreach (Video v in TempVideosB)
            {
                ListViewItem li = new ListViewItem();
                li.Text = v.File.Name;
                li.SubItems.AddRange(new string[]{v.File.Extension,v.File.Length.ToString(),v.Location.Name,Length.shortestInt(GraphM,this.comboBoxPCs.SelectedIndex,PCs.IndexOf(v.Location)).ToString()});
                lis.Add(li);
            }
            
            this.listViewSubFile.Items.AddRange(lis.OrderBy(v => v.SubItems[4].Text).ToArray());
        }
        
        

        int GetPCIntByN(string name)
        {
            return PCs.IndexOf(PCs.Find(pc => pc.Name == name.Trim(new char[] { '(', ')', ';', ',', ' ' })));
        }
       
        /// <summary>
        /// 用一个方法去遍历树
        /// </summary>
        /// <param name="node">开始节点</param>
        /// <param name="videos">视频列表</param>
        /// <param name="function">函数</param>
        void WalkFileTree(TreeNode node,List<Video> videos, FunctionNode function)
        {
            foreach (TreeNode tn in node.Nodes)
            {
                WalkFileTree(tn,videos,function);
                function(tn,videos);
            }
        }
        //入口函数，将视频添加到树节点
        void TreeViewAddNodes(List<Video> videos)
        {
            foreach (TreeNode fn in treeViewFiles.Nodes)
            {
                WalkFileTree(fn, videos, new FunctionNode(AddNode));
            }
        }

        
        //功能函数，将视频添加到树节点
        void AddNode (TreeNode node,List<Video> videos)
        {
            bool isFirst = true;
            foreach (var v in videos.FindAll(e => e.File.Directory.Name == node.Text))
            {
                if (isFirst)
                {
                    node.Nodes.Clear();
                    isFirst = false;
                }
                v.Node = node;
                node.Nodes.Add(new TreeNode { Text = v.File.Name, ToolTipText = v.Location.Name, Tag = v });
            }
        }
        //将视频添加到listview中
        void addNodeToListView(TreeNode node,List<Video> videos)
        {
            if (node.Tag != null)
            {
                Video v = node.Tag as Video;
                ListViewItem li = new ListViewItem();
                li.Text = v.File.Name;
                li.SubItems.Add(v.File.Extension);
                li.SubItems.Add(v.File.Length.ToString());
                li.SubItems.Add(v.Location.Name);
                TempVideosB.Add(v);
                this.listViewSubFile.Items.Add(li);
            }
        }
        delegate void FunctionNode(TreeNode node,List<Video> videos);

        private void comboBoxPCs_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

    }

    /*相关类的声明*/
    /*PC节点*/
    class PC
    {
        public PC()
        {
            File_Kind = new List<DirectoryInfo>();
            Videos = new List<Video>();
        }
        public string Name { get; set; }
        public List<DirectoryInfo> File_Kind { get; set; }
        public List<Video> Videos { get; set; }
        public void GetVideos(string path)
        {
            Videos = new List<Video>();
            DirectoryInfo Initial_Directory = new DirectoryInfo(path + "\\" + Name);
            File_Kind.AddRange(Initial_Directory.GetDirectories());//将此该PC下的子目录信息添加到视频种类List容器末尾；
            foreach (DirectoryInfo File_Stream in Initial_Directory.GetDirectories())
            {
                foreach (var mv in File_Stream.GetFiles())
                {
                    Videos.Add(new Video { Location = this, File = mv });//在该PC下所有视频信息保存至视频List容器内；
                }
            }
        }
    }
    /*路径长度*/
    class Length
    {
        public static string shortest(int[,] g, int a, int v)
        {
            int t = v;
            int[] P = new int[8];
            int[] D = new int[8];
            const int INFINITY = 65535;
            int[] final = new int[8];
            for (int i = 0; i < 8; ++i)
            {
                D[i] = g[a, i];//路径长度
                P[i] = a;
                final[i] = 0;
            }
            final[a] = 1;//不访问起点
            for (int i = 1; i < 8; ++i)
            {
                int min = INFINITY;//当前的最短路径
                for (int w = 0; w < 8; ++w)//取最距离a最近的点v
                {
                    if ((final[w] == 0) && (D[w] < min))
                    //如果存在更短的路径，替换
                    {
                        v = w;
                        min = D[w];
                    }
                }
                final[v] = 1;
                for (int w = 0; w < 8; ++w)
                //如果v是最短路径上的点，加到P[]中
                {
                    if ((final[w] == 0) && (min + g[v, w] < D[w]))
                    {
                        D[w] = min + g[v, w];
                        P[w] = v;//从a到w最近路径，是v
                    }
                }
            }
            string r = t.ToString();
            do
            {
                r += P[t].ToString();
                t = P[t];
            } while (t != a);//读出反向路径
            //倒置字符串
            char[] chararray = r.ToCharArray();
            Array.Reverse(chararray);
            return new string(chararray);
        }
        public static int shortestInt(int[,] g, int s, int v)
        {
            int result = 0;
            char[] path = shortest(g, s, v).ToArray();
            for (int i = 0; i < path.Length - 1; i++)
            {
                result += g[int.Parse(path[i].ToString()), int.Parse(path[i + 1].ToString())];
            }
            return result;

        }
    }
    /*遍历图节点*/
    class GraphNode
    {
        public GraphNode(PC n1)
        {
            Nodes = new List<GraphNode>();
            Node1 = n1;
        }
        public PC Node1 { get; set; }
        public List<GraphNode> Nodes { get; set; }
        public int Rate { get; set; }
    }
    /*视频*/
    class Video
    {
        public FileInfo File { get; set; }
        public TreeNode Node { get; set; }
        public PC Location { get; set; }
    }
}
