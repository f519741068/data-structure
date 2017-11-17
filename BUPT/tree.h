/*
 * 这是董昊(2016210772)第二个数据结构实验的代码，它是一个红黑树(Red Black Tree)
 *
 * 这棵树也是一个字典，每个节点中都有一个key和value，它们一一对应，不可重复。
 *
 * 如果用户想要添加具有相同key的不同value，可以使用tree<key,vector<value>>声明对象。
 *
 * 在红黑树中，最长的路径的长度不会超过最短路径的长度的2倍，因此它大体上是平衡的，不会有哪条路径长的离谱
 *
 * 在实际的测试中，我发现这棵树在性能上表现优异，但绝大多数情况下它也做不到完全平衡（即叶子节点的最长路径与最短路径的长度差不超过1）
 *
 * 分别用红黑树和普通二叉搜索树对共产党宣言(communist.txt)进行词频统计，在乱序的情况下，它们性能差不多
 *
 * 然后对词汇进行排序再统计，二者性能相差接近一百倍
 */

#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <algorithm>
#include <queue>
using namespace std;

template<class Key,class Value>
class tree{
protected:
    struct node{
        Key key;
        Value value;
        node *parent;//指向父节点
        node *lc;//指向左孩子
        node *rc;//指向右孩子
        int leftcount;//左子树中有多少个非叶子结点
        int rightcount;//右子树中有多少个非叶子结点
        bool black;//true说明该节点为黑色，false说明该节点为红色
    };
    int Size;//这棵树中有多少个非叶子结点
    node *root;

    void destroy(node *n)//销毁以n所指向的节点为根的整棵树，仅供内部调用，时间复杂度为O(n)
    {
        if(n)
        {
            destroy(n->lc);
            destroy(n->rc);
            delete n;
        }
    }

    bool leftRotate(node *pivot)//以pivot所指向的节点为中心左旋，仅供内部调用，时间复杂度为O(1)
    {
        if(NULL==pivot->rc)
          return false;
        if(pivot==root)
          root=pivot->rc;
        int t=pivot->rc->leftcount;
        pivot->rc->leftcount+=pivot->leftcount+1;
        pivot->rightcount=t;
        if(pivot->parent)
        {
            if(pivot==pivot->parent->lc)
              pivot->parent->lc=pivot->rc;
            else
              pivot->parent->rc=pivot->rc;
        }
        pivot->rc->parent=pivot->parent;
        node *temp=pivot->rc->lc;
        pivot->parent=pivot->rc;
        pivot->rc->lc=pivot;
        pivot->rc=temp;
        if(temp)
          temp->parent=pivot;
        return true;
    }

    bool rightRotate(node *pivot)//以pivot所指向的节点为中心右旋，仅供内部调用，时间复杂度为O(1)
    {
        if(NULL==pivot->lc)
          return false;
        if(pivot==root)
          root=pivot->lc;
        int t=pivot->lc->rightcount;
        pivot->lc->rightcount+=pivot->rightcount+1;
        pivot->leftcount=t;
        if(pivot->parent)
        {
            if(pivot==pivot->parent->lc)
              pivot->parent->lc=pivot->lc;
            else
              pivot->parent->rc=pivot->lc;
        }
        pivot->lc->parent=pivot->parent;
        node *temp=pivot->lc->rc;
        pivot->parent=pivot->lc;
        pivot->lc->rc=pivot;
        pivot->lc=temp;
        if(temp)
          temp->parent=pivot;
        return true;
    }

    void preOrder(node *p)//前序遍历以p所指向的节点为根的整棵子树，仅供内部调用，时间复杂度为O(n)
    {
        if(p)
        {
            cout << "Key:" << p->key << "   Value:" << p->value <<endl;
            preOrder(p->lc);
            preOrder(p->rc);
        }
    }

    void inOrder(node *p)//中序遍历以p所指向的节点为根的整棵子树，仅供内部调用，时间复杂度是O(n)
    {
        if(p)
        {
            inOrder(p->lc);
            cout << "Key:" << p->key << "   Value:" << p->value <<endl;
            inOrder(p->rc);
        }
    }

    void postOrder(node *p)//后续便利以p所指向的节点为根的整棵子树，仅供内部调用，时间复杂度是O(1)
    {
        if(p)
        {
            postOrder(p->lc);
            postOrder(p->rc);
            cout << "Key:" << p->key << "   Value:" << p->value <<endl;
        }
    }

    void copy(node* &i,node *o)//被拷贝构造函数调用，拷贝以o所指向的节点为根的整棵子树，仅供内部调用，时间复杂度是O(n)
    {
        if(o)
        {
            i=new node;
            *i=*o;
            copy(i->rc,o->rc);
            copy(i->lc,o->lc);
        }
    }

    void Delete(node *p)//供public中的pop函数调用，删除p所指向的节点，然后调用repair函数，修复被破坏的红黑树性质，仅供内部调用，时间复杂度是O(log n)
    {
        node *temp;
        bool d;
        if(p->rc&&p->lc)
        {
            if(p->leftcount>p->rightcount)
            {
                p->leftcount--;
                temp=p->lc;
                while(temp->rc)
                {
                    temp->rightcount--;
                    temp=temp->rc;
                }
            }
            else
            {
                p->rightcount--;
                temp=p->rc;
                while(temp->lc)
                {
                    temp->leftcount--;
                    temp=temp->lc;
                }
            }
            p->key=temp->key;
            p->value=temp->value;
            if(temp==temp->parent->lc)
              temp->parent->lc=NULL;
            else
              temp->parent->rc=NULL;
            Delete(temp);
        }
        else
        {
            temp=p->lc?p->lc:p->rc;
            if(temp)
              temp->parent=p->parent;
            if(p->parent)
            {
                if(p==p->parent->lc)
                {
                    p->parent->lc=temp;
                    d=false;
                }
                else
                {
                    p->parent->rc=temp;
                    d=true;
                }
            }
            else
            {
                root=temp;
                if(temp)
                  temp->black=true;
                delete p;
                return ;
            }
            bool flag=p->black;
            temp=p->parent;
            delete p;
            p=NULL;
            if(flag)
              repair(temp,d);
        }
    }

    void repair(node *p,bool d)//供Delete函数调用，修复被破坏的红黑树性质，p指向被删除节点的父节点（如果有的话），d表明被删除的节点是父节点的左孩子还是右孩子（false为左，true为右），仅供内部调用，时间复杂度是O(log n)
    {
        if(NULL==p)
          return ;
        node *n,*s,*sl,*sr;
        bool bp,bn=true,bs=true,bl=true,br=true;
        if(d)
        {
            n=p->rc;
            s=p->lc;
        }
        else
        {
            n=p->lc;
            s=p->rc;
        }
        if(s)
        {
          sl=s->lc;
          sr=s->rc;
          bs=s->black;
        }
        if(n)
        {
          bn=n->black;
        }
        bp=p->black;
        if(sl)
          bl=sl->black;
        if(sr)
          br=sr->black;
        if(n)
          bn=n->black;
        if(!bn)
        {
            n->black=true;
            return ;
        }
        if(bs&&bp&&bl&&br)
        {
            s->black=false;
            if(!p->parent)
              return ;
            if(p==p->parent->rc)
              d=true;
            else
              d=false;
            repair(p->parent,d);
            return ;
        }
        if((!bp)&&bs&&bl&&br)
        {
            p->black=true;
            s->black=false;
            return ;
        }
        if(bs&&(!br))
        {
            sr->black=true;
            swap(p->black,s->black);
            leftRotate(p);
            return ;
        }
        if(bs&&(!bl))
        {
            s->black=false;
            sl->black=true;
            rightRotate(s);
            return ;
        }
        if(!bs)
        {
            s->black=true;
            p->black=false;
            leftRotate(p);
            return ;
        }
    }
public:
    tree()//空构造函数，时间复杂度是O(1)
    {
        Size=0;
        root=NULL;
    }

    tree(tree<Key,Value> &o)//拷贝构造函数，时间复杂度是O(n)
    {
        copy(root,o.root);//通过递归的方法创建
    }

    Value* search(Key k)//既是查找函数，也是修改函数，时间复杂度是O(log n)
    {
        node *p=root;
        while(p)
        {
            if(k<p->key)
              p=p->lc;
            else if(k==p->key)
              return &(p->value);
            else
              p=p->rc;
        }
        return NULL;
    }

    bool empty()//是否为空，时间复杂度是O(1)
    {
        return Size==0;
    }

    int size()//返回整棵数有多少非叶子结点，时间复杂度是O(1)
    {
        return Size;
    }

    bool contain(Key k)//判断指定的key是否在这棵树中，时间复杂度是O(log n)
    {
        node *p=root;
        while(p)
        {
            if(k<p->key)
              p=p->lc;
            else if(k>p->key)
              p=p->rc;
            else if(k==p->key)
              return true;
        }
        return false;
    }

    void clear()//清空整棵数，时间复杂度是O(n)
    {
        if(root)
          destroy(root);//通过递归的方法删掉所有节点
        root=NULL;
        Size=0;
    }

    bool insert(Key k,Value v)//插入一个结点，并保持红黑树性质不变，时间复杂度是O(log n)
    {
        node *temp=new node;
        if(NULL==temp)
          return false;
        temp->key=k;
        temp->value=v;
        temp->black=false;
        temp->parent=temp->lc=temp->rc=NULL;
        temp->leftcount=temp->rightcount=0;
        if(NULL==root)
        {
            root=temp;
            root->black=true;
            Size++;
            return true;
        }

        if(contain(k))
          return false;

        node *p1=root;
        node *p2=NULL;
        while(p1)
        {
            p2=p1;
            if(k<p1->key)
            {
                p1->leftcount++;
                p1=p1->lc;
            }
            else
            {
                p1->rightcount++;
                p1=p1->rc;
            }
        }
        if(k<p2->key)
          p2->lc=temp;
        else
          p2->rc=temp;
        temp->parent=p2;
        p1=p2;
        p2=p2->parent;
        Size++;
        while(true)
        {
            if(p1->black)
              return true;
            else if(NULL==p2->lc)
            {
                leftRotate(p2);
                p2->black=false;
                p1->black=true;
                return true;
            }
            else if(NULL==p2->rc)
            {
                rightRotate(p2);
                p2->black=false;
                p1->black=true;
                return true;
            }
            else if(false==p2->lc->black&&false==p2->rc->black)
            {
                p2->lc->black=p2->rc->black=true;
                p2->black=false;
                temp=p2;
                p1=temp->parent;
                if(p1)
                  p2=p1->parent;
                else
                {
                    temp->black=true;
                    return true;
                }
                if(NULL==p2)
                  return true;
            }
            else if(temp==p1->lc&&p1==p2->lc)
            {
                rightRotate(p2);
                p1->black=true;
                p2->black=false;
                return true;
            }
            else if(temp==p1->rc&&p1==p2->rc)
            {
                leftRotate(p2);
                p1->black=true;
                p2->black=false;
                return true;
            }
            else if(temp==p1->lc&&p1==p2->rc)
            {
                rightRotate(p1);
                swap(p1,temp);
            }
            else if(temp==p1->rc&&p1==p2->lc)
            {
                leftRotate(p1);
                swap(p1,temp);
            }
        }
        return false;
    }

    bool pop(Key k)//删掉一个结点，并保持红黑树性质不变，时间复杂度是O(log n)
    {
        node *p=root;
        if(!contain(k))
          return false;
        while(p)
        {
            if(k<p->key)
            {
                p->leftcount--;
                p=p->lc;
            }
            else if(k>p->key)
            {
                p->rightcount--;
                p=p->rc;
            }
            else if(k==p->key)
            {
                Size--;
                Delete(p);
                return true;
            }
        }
        return false;
    }

    ~tree()//析构函数，时间复杂度是O(n)
    {
        destroy(root);//递归删掉所有节点
    }

    void preOrder()//前序遍历，时间复杂度是O(n)
    {
        preOrder(root);//递归遍历
    }

    void inOrder()//中序遍历，时间复杂度是O(n)
    {
        inOrder(root);//递归遍历
    }

    void postOrder()//后续遍历，时间复杂度是O(n)
    {
        postOrder(root);//递归遍历
    }

    void levelOrder()//层序遍历，时间复杂度是O(n)
    {
        if(NULL==root)
          return ;
        node *p;
        queue<node*> q;//使用队列遍历
        q.push(root);
        while(!q.empty())
        {
            p=q.front();
            q.pop();
            cout << "Key:" << p->key << "   Value:" << p->value <<endl;
            if(p->lc)
              q.push(p->lc);
            if(p->rc)
              q.push(p->rc);
        }
    }

    Value* select(int n)//返回正数第n大的key所对应的value的指针，时间复杂度是O(log n)
    {
        if(n>Size||0==n)
          return NULL;
        n--;
        node *p=root;
        while(n!=p->rightcount)
        {
            if(n<p->rightcount)
              p=p->rc;
            else
            {
                n-=p->rightcount+1;
                p=p->lc;
            }
        }
        return &(p->value);
    }

    Value* iselect(int n)//返回倒数第n大的key所对应的value指针，时间复杂度是O(log n)
    {
        if(n>Size||0==n)
          return NULL;
        n--;
        node *p=root;
        while(n!=p->leftcount)
        {
            if(n<p->leftcount)
              p=p->lc;
            else
            {
                n-=p->leftcount+1;
                p=p->rc;
            }
        }
        return &(p->value);
    }

    int rank(Key k)//传入一个key，看这个key可以在树中排到正数第几大，时间复杂度是O(log n)
    {
        int r=1;
        node *p=root;
        while(p)
        {
            if(k==p->key)
              return r+p->rightcount;
            else if(k>p->key)
              p=p->rc;
            else
            {
                r+=p->rightcount+1;
                p=p->lc;
            }
        }
        return r;
    }

    int irank(Key k)//传入一个key，看这个key可以在树中排到倒数第几大，时间复杂度是O(log n)
    {
        int r=1;
        node *p=root;
        while(p)
        {
            if(k==p->key)
              return r+p->leftcount;
            else if(k<p->key)
              p=p->lc;
            else
            {
                r+=p->leftcount+1;
                p=p->rc;
            }
        }
        return r;
    }
};

#endif