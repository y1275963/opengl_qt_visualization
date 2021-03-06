#include "filereader.h"

filereader::filereader(float parameter, int jump):
    jump(jump)
{
    reader_table();//read table
    reader_raw();//read raw data
    interpolation_generator(parameter);
}

float filereader::readfloat(FILE *f){
    //used for read signle data from raw file
    float v;
    fread((void*)(&v),sizeof(v),1,f);
    return v;
}

void filereader::reader_raw(){
    //read the whole raw file
    FILE * file = fopen("/Users/y1275963/SciVis/scalarGFull.raw","r");
    for(int z=0;z<zsize;z++){
        for(int y=0;y<ysize;y++){
            for(int x=0;x<xsize;x++){
                data[x][y][z] = readfloat(file);
            }
        }
    }
}

void filereader::reader_table(){
    //read from the table for condition & solution
    QFile file("/Users/y1275963/SciVis/table.txt");
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(0,"error",file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList fields = line.split("\t");
        QStringListIterator iter(fields);
        while(iter.hasNext()){
            table.insert(iter.next(),iter.next());
        }
    }
 //   qDebug()<<table.value(QString("0567")).trimmed().split(" ");
}

float filereader::interpolation_single(float start_value, float end_vaule, float start_location, float end_location, float parameter){
    // generall interpolation
    return start_location + (end_location-start_location)*(parameter-start_value)/(end_vaule-start_value);
}

QVector3D filereader::interpolation_3d(QVector3D start, QVector3D end, float parameter, float start_value, float end_value){
   // Interpolation along 3 axises
    float xtemp = interpolation_single(start_value,end_value,start.x(),end.x(),parameter);
    float ytemp = interpolation_single(start_value,end_value,start.y(),end.y(),parameter);
    float ztemp = interpolation_single(start_value,end_value,start.z(),end.z(),parameter);
    return QVector3D(xtemp,ytemp,ztemp);
}

void filereader::interpolation_generator(float parameter){
    // Main list generator function:
    // 1. Using gen_cubic to retrive data
    // 2. Using gen_condition to get the condition code
    // 3. check the table, refer to each solution and do the interpolation using call_con
    // 4. push the data into a container called drawlist
    QList<float> cubic_temp;
    QString condition; 

    for(int x=0;x<xsize-1-jump;x+=jump){
        for(int y=0;y<ysize-1-jump;y+=jump){
            for(int z=0;z<zsize-1-jump;z+=jump){
                cubic_temp = gen_cubic(x,y,z);
                condition = gen_condition(parameter,cubic_temp);
              //  qDebug()<<condition;
                QStringList con_temp = table.value(condition).trimmed().split(" ");
                QStringListIterator iter(con_temp);

               //qDebug()<<x<<y<<z;
                while(iter.hasNext()){
                  //  qDebug()<<iter.next();
                    drawlist << call_con(iter.next(),QVector3D::QVector3D(x,y,z),parameter,cubic_temp);
                }
        }
    }
}
}
QList<QVector3D> filereader::call_con(QString con, QVector3D starter, float parameter, QList<float> cubic){
   // Genearting 3 vectors data. single_call_con is the sub function
    // split the 6 number in 3 gourp. Process individually
    // sum up in a list.
    QList<QVector3D> val;
    QString con_vec1 = con.mid(0,2);
    val << single_call_con(con_vec1,starter,parameter,cubic);
    QString con_vec2 = con.mid(2,2);
    val << single_call_con(con_vec2,starter,parameter,cubic);
    QString con_vec3 = con.mid(4,2);
    val << single_call_con(con_vec3,starter,parameter,cubic);
    return val;
}

QVector3D filereader::single_call_con(QString con, QVector3D starter,float parameter,QList<float> cubic){
   // Refer to the condition to do the single interpolation, generate a single vertex data
    // 1. using container to store the x,y,z coordinates from start -> end
    // 2. using condition to store the start_vectex_index -> end_vertex_index
    // 3. using interpolation_3d to do the interpolation
    // 4. return single vertex 3d coordinates
    QList<QVector3D> container;
    QList<int> condition; //higher cubic index
    condition << con.left(1).toInt();
    condition << con.right(1).toInt();

    QList<int>::iterator iter;
    float xvalue = starter.x();
    float yvalue = starter.y();
    float zvalue = starter.z();
    for(iter = condition.begin(); iter!= condition.end();++iter){
        switch (*iter) {
        case 0:
            container << starter;
            break;
        case 1:
            container << QVector3D::QVector3D(xvalue+1,yvalue,zvalue);
            break;
        case 2:
            container << QVector3D::QVector3D(xvalue+1,yvalue+1,zvalue);
            break;
        case 3:
            container << QVector3D::QVector3D(xvalue,yvalue+1,zvalue);
            break;
        case 4:
            container << QVector3D::QVector3D(xvalue,yvalue,zvalue+1);
            break;
        case 5:
            container << QVector3D::QVector3D(xvalue+1,yvalue,zvalue+1);
            break;
        case 6:
            container << QVector3D::QVector3D(xvalue+1,yvalue+1,zvalue+1);
            break;
        case 7:
            container << QVector3D::QVector3D(xvalue,yvalue+1,zvalue+1);
            break;
        }
    }
    return interpolation_3d(container[0],container[1],parameter,cubic[condition[0]],cubic[condition[1]]); // The last two is to get the value
}


QString filereader::gen_condition(float parameter,QList<float> cubic){
    // According to the cubic value. According to the parameter. Finding the vertex index which is higher that the parameter
    // transfer the coordinators into string, which is the condition indicator.
    QList<int> con_list;
    QString string;

    for(int iter =0; iter<=7 ;iter++){
        if(cubic[iter] >= parameter){
            con_list << iter;
        }
    }


    for(int i =0;i< con_list.size();i++){
        string += QString::number(con_list[i]);
    }
    return string;
}

QList<float> filereader::gen_cubic( int x, int y, int z){
    // get the vertex value. Inefficient.
    QList<float> cubic;
    float val0 = data[x][y][z];
    float val1 = data[x+1][y][z];
    float val4 = data[x][y][z+1];
    float val3 = data[x][y+1][z];
    float val7 = data[x][y+1][z+1];
    float val6 = data[x+1][y+1][z+1];
    float val5 = data[x+1][y][z+1];
    float val2 = data[x+1][y+1][z];

    cubic<<val0<<val1<<val2<<val3<<val4<<val5<<val6<<val7;
    return cubic;
}

