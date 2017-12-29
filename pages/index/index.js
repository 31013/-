Page({
  data: {

    opacity: 0.4,

    disabled: true,

    threshold: 0,

    rule: 'up',


  },
  
 
  getDataFromOneNet: function () {
    const requestTask = wx.request({

      url: 'https://api.heclouds.com/devices/20449602/datapoints?datastream_id=Light&limit=1',

      header: {

        'content-type': 'application/json',

        'api-key': 'RFeyfLJTSW6WcEeWSPIfmD5c8Xo='

      },
      success: function (res) {



        //console.log(res.data)

        //拿到数据后保存到全局数

        var light = res.data.data.datastreams[0]

        // console.log(light.datapoints[0].value)
        if (light.datapoints[0].value >= 700) {
          wx.showModal({


            title: '警报！',

            content: '有人来了',

            success: function (res) {

              if (res.confirm) {

                console.log('用户点击确定')

              } else if (res.cancel) {

                console.log('用户点击取消')

              }

            }

          })

        }

      },


      fail: function (res) {

        console.log("fail!!!")

      },


      complete: function (res) {

        console.log("end")

      }


    })


  },

   

 /**
  
  * 生命周期函数--监听页面初次渲染完成
  
  */

   onReady: function () {
    

  },


  /**
  
  * 生命周期函数--监听页面显示
  
  */

   onShow: function () {
     setInterval(function(){
       const requestTask = wx.request({

         url: 'https://api.heclouds.com/devices/20449602/datapoints?datastream_id=Light&limit=1',

         header: {

           'content-type': 'application/json',

           'api-key': 'RFeyfLJTSW6WcEeWSPIfmD5c8Xo='

         },
         success: function (res) {



           //console.log(res.data)

           //拿到数据后保存到全局数

           var light = res.data.data.datastreams[0]

            console.log(light.datapoints[0].value)
           if (light.datapoints[0].value >= 700) {
             wx.showModal({


               title: '警报！',

               content: '有人来了',

               success: function (res) {

                 if (res.confirm) {

                   console.log('用户点击确定')

                 } else if (res.cancel) {

                   console.log('用户点击取消')

                 }

               }

             })

           }

         },


         fail: function (res) {

           console.log("fail!!!")

         },


         complete: function (res) {

           console.log("end")

         }


       })

     },3000)
    
  },


  /**
  
  * 生命周期函数--监听页面隐藏
  
  */

  onHide: function () {


  },


  /**
  
  * 生命周期函数--监听页面卸载
  
  */

  onUnload: function () {


  },


  /**
  
  * 页面相关事件处理函数--监听用户下拉动作
  
  */

  onPullDownRefresh: function () {


  },


  /**
  
  * 页面上拉触底事件的处理函数
  
  */

  onReachBottom: function () {


  },


  /**
  
  * 用户点击右上角分享
  
  */

  onShareAppMessage: function () {


  },

  

})
