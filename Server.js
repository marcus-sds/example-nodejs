var express = require("express");
var app = express();
var router = express.Router();
var path = __dirname + '/views/';

router.use(function (req,res,next) {
  console.log("/" + req.method);
  next();
});

router.get("/",function(req,res){
  res.sendFile(path + "index.html");
});

router.get("/about",function(req,res){
  res.sendFile(path + "about.html");
});

router.get("/discount",function(req,res){
  res.sendFile(path + "discount.html");
});

router.get("/search",function(req,res){
  res.sendFile(path + "search.html");
});

router.get("/congestion",function(req,res){
  res.sendFile(path + "congestion.html");
});

app.use(express.static(__dirname + '/public'));


app.use("/",router);


app.listen(9000,function(){
  console.log("Live at Port 9000");
});
